import java.util.ArrayList;

public class CengTreeNodeLeaf extends CengTreeNode
{
    private ArrayList<CengBook> books;
    // TODO: Any extra attributes

    public CengTreeNodeLeaf(CengTreeNode parent)
    {
        super(parent);

        this.type = CengNodeType.Leaf;
        this.books = new ArrayList<>();

        // TODO: Extra initializations
    }

    // GUI Methods - Do not modify
    public int bookCount()
    {
        return books.size();
    }
    public Integer bookKeyAtIndex(Integer index)
    {
        if(index >= this.bookCount()) {
            return -1;
        } else {
            CengBook book = this.books.get(index);

            return book.getBookID();
        }
    }

    // Extra Functions

    public ArrayList<CengBook> getAllBooks()
    {
        return this.books;
    }

   @Override
    public void addBook(CengBook book, CengTree tree){
        int size = this.bookCount();
           if( size == 0 || this.bookKeyAtIndex(size-1) <= book.getBookID()){
               this.books.add(book);
           }
           else {
               size = this.bookCount();
               for (int i = 0; i < books.size(); i++) {
                   if (book.getBookID() < this.bookKeyAtIndex(i)) {
                       this.books.add(i, book);
                       break;
                   }
               }
           }

       size = this.bookCount();
           if(size > 2 * this.order){
               CengTreeNodeLeaf newNode = new CengTreeNodeLeaf(this.getParent());
               int middleIndex = size / 2;

               for (int i = middleIndex; i < size; i++) {
                   newNode.books.add(this.books.get(i));
               }
               for(int i=this.books.size()-1;i>=middleIndex;i--) {
                   this.books.remove(i);
               }

               if(this.getParent() == null){
                   CengTreeNodeInternal t_root = new CengTreeNodeInternal(null);
                   tree.root = t_root;
                   t_root.addPointer(this, this.bookKeyAtIndex(0), newNode, newNode.bookKeyAtIndex(0), tree);
               }
               else{
                   ((CengTreeNodeInternal)getParent()).addPointer(null,0, newNode, newNode.bookKeyAtIndex(0), tree);
               }
           }
    }
}
