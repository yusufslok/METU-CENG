import re
import os
from haystack.document_stores import InMemoryDocumentStore
from haystack.nodes import BM25Retriever, EmbeddingRetriever, FARMReader
from haystack.pipelines import ExtractiveQAPipeline
from haystack.schema import Document
import torch
import gc
from groq import Groq


class QAProcessor:
    def __init__(self, raw_file_path):
        # Retrieve API key from environment variable
        self.api_key = "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"

        # Initialize Groq client
        self.client = Groq(api_key=self.api_key)

        # Preprocess data
        processed_documents = self.preprocess_data(raw_file_path)

        # Initialize document store
        self.document_store = InMemoryDocumentStore(
            use_bm25=True,
            similarity="cosine",
            return_embedding=True,
            embedding_dim=768
        )

        # Initialize retrievers and reader
        self.bm25_retriever, self.embedding_retriever = self.initialize_retrievers()

        self.ingest_documents(processed_documents)

        self.reader = self.initialize_reader()

        # Create pipeline
        self.pipeline = self.create_pipeline()

    def preprocess_data(self, file_path):
        documents = self.parse_raw_data(file_path)
        processed_docs = []
        for doc in documents:
            clean_content = self.clean_text(doc['content'])
            for chunk in self.chunk_document(clean_content):
                processed_docs.append(Document(content=chunk, meta={'url': doc['url']}))
        return processed_docs

    def parse_raw_data(self, file_path):
        with open(file_path, 'r', encoding='utf-8') as f:
            data = f.read()

        entries = re.split(r'URL:', data)[1:]
        documents = []
        for entry in entries:
            match = re.match(r'\s*(\S+)\s+(.*)', entry, re.DOTALL)
            if match:
                url = match.group(1).strip()
                content = match.group(2).strip()
                documents.append({"url": url, "content": content})
        return documents

    def clean_text(self, text):
        text = re.sub(r'<[^>]+>', '', text)
        text = re.sub(r'\s+', ' ', text)
        return text.strip()

    def chunk_document(self, content, chunk_size=500):
        words = content.split()
        for i in range(0, len(words), chunk_size):
            yield ' '.join(words[i:i + chunk_size])

    def ingest_documents(self, documents):
        self.document_store.write_documents(documents)
        #self.document_store.update_embeddings(retriever=self.embedding_retriever)

    def initialize_retrievers(self):
        bm25_retriever = BM25Retriever(document_store=self.document_store)
        embedding_retriever = EmbeddingRetriever(
            document_store=self.document_store,
            embedding_model="sentence-transformers/multi-qa-mpnet-base-dot-v1",
            model_format="sentence_transformers",
            use_gpu=True,
        )
        return bm25_retriever, embedding_retriever

    def initialize_reader(self):
        torch.cuda.empty_cache()
        gc.collect()
        return FARMReader(model_name_or_path="deepset/xlm-roberta-base-squad2", use_gpu=True)

    def create_pipeline(self):
        return ExtractiveQAPipeline(reader=self.reader, retriever=self.bm25_retriever)

    def read_and_answer(self, prompt):
        response = self.client.chat.completions.create(
            model="llama-3.3-70b-specdec",
            messages=[
                {"role": "system", "content": "I will give you a question and some documents. You should answer the question using documents. Do not mention provided documents in your answer."},
                {"role": "user", "content": prompt}
            ],
            max_tokens=128
        )
        generated_text = response.choices[0].message.content
        return generated_text

    def ask_question(self, query, top_k_reader=5):
        result = self.pipeline.run(query=query, params={"Retriever": {"top_k": top_k_reader}, "Reader": {"top_k": top_k_reader}})
        contents = []
        for answer in result['answers']:
            print(f"\nAnswer: {answer.answer}\nScore: {answer.score:.2f}\nSource: {answer.meta.get('url', 'N/A')}\n")
            contents.append(self.document_store.get_document_by_id(answer.document_ids[0]).content)
        prompt = "\n\n".join(contents)
        prompt = query + "\n\n" + prompt
        generated_text = self.read_and_answer(prompt)
        return generated_text