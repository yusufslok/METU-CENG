import java.util.ArrayList;

public class CengTree
{
    public CengTreeNode root;
    // Any extra attributes...

    public CengTree(Integer order)
    {
        CengTreeNode.order = order;
        // TODO: Initialize the class
        this.root = new CengTreeNodeLeaf(null);
    }

    public void addBook(CengBook book)
    {
        this.root.addBook(book, this);
    }

    public ArrayList<CengTreeNode> searchBook(Integer bookID)
    {
        // TODO: Search within whole Tree, return visited nodes.
        // Return null if not found.
        ArrayList<CengTreeNode> visitedNodes1 = new ArrayList<CengTreeNode>();
        ArrayList<CengTreeNode> visitedNodes = new ArrayList<CengTreeNode>();
        visitedNodes = searchBookRec(root, bookID, visitedNodes1, 0);

        String str = "\t";
        int pad=0;
        if(visitedNodes != null){
            for(int i=0;i<visitedNodes.size()-1;i++){
                CengTreeNodeInternal InternalNode = (CengTreeNodeInternal) visitedNodes.get(i);
                int size = ((CengTreeNodeInternal)InternalNode).keyCount();
                System.out.println( str.repeat(pad) + "<index>");
                for(int j=0;j<size;j++){
                    System.out.println( str.repeat(pad) + InternalNode.keyAtIndex(j));
                }
                System.out.println( str.repeat(pad) + "</index>");
                pad++;
            }
            int size1 = ((CengTreeNodeLeaf) visitedNodes.get(visitedNodes.size() - 1)).bookCount();
            CengTreeNodeLeaf nodee = ((CengTreeNodeLeaf) visitedNodes.get(visitedNodes.size() - 1));
            for(int j=0;j<size1;j++){
                if(bookID == nodee.bookKeyAtIndex(j)){
                    System.out.print( str.repeat(pad) + "<record>");
                    System.out.print(nodee.bookKeyAtIndex(j) + "|");
                    System.out.print(nodee.getAllBooks().get(j).getBookTitle()+"|");
                    System.out.print(nodee.getAllBooks().get(j).getAuthor()+"|");
                    System.out.print(nodee.getAllBooks().get(j).getGenre());
                    System.out.println("</record>");
                }
            }
        }
        else{
            System.out.print("Could not find ");System.out.print(bookID);System.out.print(".");
        }

        return visitedNodes;
    }

    public ArrayList<CengTreeNode> searchBookRec(CengTreeNode node, Integer bookID, ArrayList<CengTreeNode> arr, int pad){
        arr.add(node);
        if(node.type == CengNodeType.Internal){
            CengTreeNodeInternal InternalNode = ((CengTreeNodeInternal)node);
            ArrayList<CengTreeNode> childs = InternalNode.getAllChildren();
            int size = InternalNode.keyCount();
            if(bookID >= InternalNode.keyAtIndex(size-1)){
                return searchBookRec(childs.get(size), bookID, arr, pad+1);
            }
            for(int i=0;i<InternalNode.keyCount();i++){
                if(bookID < InternalNode.keyAtIndex(i)){
                    return searchBookRec(childs.get(i), bookID, arr,pad+1);
                }
            }
        }
        else if(node.type == CengNodeType.Leaf){
            CengTreeNodeLeaf leafNode = ((CengTreeNodeLeaf)node);
            ArrayList<CengBook> books = leafNode.getAllBooks();
            for(int i=0;i<leafNode.bookCount();i++){
                if(bookID.equals(leafNode.bookKeyAtIndex(i))){
                    return arr;
                }
            }
            return null;
        }
        return null;
    }
    public void printTree()
    {
        printrec(root, 0);

    }

    public void printrec(CengTreeNode node, int pad){
        String str = "\t";
        if(node.type == CengNodeType.Internal){
            int size = ((CengTreeNodeInternal)node).keyCount();
            CengTreeNodeInternal InternalNode = (CengTreeNodeInternal) node;
            System.out.println( str.repeat(pad) + "<index>");
            for(int i=0;i<size;i++){
                System.out.println( str.repeat(pad) + InternalNode.keyAtIndex(i));
            }
            System.out.println( str.repeat(pad) + "</index>");
            ArrayList<CengTreeNode> childs = InternalNode.getAllChildren();
            for(int i=0 ;i<childs.size();i++){
                printrec(childs.get(i), pad+1);
            }
        }
        else{
            System.out.println( str.repeat(pad) + "<data>");

            CengTreeNodeLeaf LeafNode = (CengTreeNodeLeaf) node;
            ArrayList<CengBook> Books = LeafNode.getAllBooks();
            for(int i=0;i<Books.size();i++){
                System.out.print( str.repeat(pad) + "<record>");
                System.out.print(LeafNode.bookKeyAtIndex(i) + "|");
                System.out.print(Books.get(i).getBookTitle()+"|");
                System.out.print(Books.get(i).getAuthor()+"|");
                System.out.print(Books.get(i).getGenre());
                System.out.println("</record>");
            }
            System.out.println( str.repeat(pad) + "</data>");
        }
    }


    // Any extra functions...
}
