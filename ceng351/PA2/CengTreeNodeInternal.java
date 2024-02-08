import java.util.ArrayList;

public class CengTreeNodeInternal extends CengTreeNode
{
    private ArrayList<Integer> keys;
    private ArrayList<CengTreeNode> children;

    public CengTreeNodeInternal(CengTreeNode parent)
    {
        super(parent);

        // TODO: Extra initializations, if necessary.

        this.type = CengNodeType.Internal;
        this.keys = new ArrayList<>();
        this.children = new ArrayList<>();
    }

    // GUI Methods - Do not modify
    public ArrayList<CengTreeNode> getAllChildren()
    {
        return this.children;
    }
    public Integer keyCount()
    {
        return this.keys.size();
    }
    public Integer keyAtIndex(Integer index)
    {
        if(index >= this.keyCount() || index < 0)
        {
            return -1;
        }
        else
        {
            return this.keys.get(index);
        }
    }

    // Extra Functions

    public void addPointer(CengTreeNode oldNode, Integer oldKey, CengTreeNode new_Node, Integer newKey, CengTree tree){
        if(oldNode != null){
            children.add(0, oldNode);
            keys.add(0, newKey);
            children.add(1, new_Node);
            new_Node.setParent(this);
            oldNode.setParent(this);
            return;
        }
        int size =  keyCount();
        if(size == 0 || keyAtIndex(size-1) <= newKey){
            keys.add(newKey);
            children.add(new_Node);
        }
        else{
            for(int i=0;i<size;i++){
                if(newKey < keyAtIndex(i)){
                    keys.add(i, newKey);
                    break;
                }
            }
            size =  keys.size();
            for(int i=0;i<size;i++){
                if(newKey < keyAtIndex(i)){
                    children.add(i, new_Node);
                    break;
                }
            }

        }
        new_Node.setParent(this);
        size = keys.size();
        if(size > 2 * this.order){
            CengTreeNodeInternal newNode = new CengTreeNodeInternal(this.getParent());
            int middleIndex = size / 2;
            int midKey = keyAtIndex(middleIndex);

            for (int i = middleIndex+1; i < size; i++) {
                newNode.keys.add(this.keys.get(i));
            }
            for(int i=this.keys.size()-1;i>=middleIndex;i--) {
                this.keys.remove(i);
            }
            for(int i=middleIndex+1;i<children.size();i++){
                newNode.children.add(children.get(i));
            }
            for(int i=0; i<newNode.children.size();i++){
                newNode.children.get(i).setParent(newNode);
            }
            for(int i=children.size()-1;i>=middleIndex+1;i--){
                children.remove(i);
            }
            if(this.getParent() == null){
                CengTreeNodeInternal t_root = new CengTreeNodeInternal(null);
                tree.root = t_root;
                t_root.addPointer(this, this.keyAtIndex(0), newNode, midKey, tree);
            }
            else{
                ((CengTreeNodeInternal)getParent()).addPointer(null,0, newNode, midKey, tree);
            }
        }
    }
    @Override
    public void addBook(CengBook book, CengTree tree){
        int size = this.keys.size();
        if(this.keyAtIndex(size-1) <= book.getBookID()){
            this.children.get(size).addBook(book, tree);
        }
        else {
            size = this.keys.size();
            for (int i = 0; i < size; i++) {
                if (book.getBookID() < this.keyAtIndex(i)) {
                    this.children.get(i).addBook(book, tree);
                    break;
                }
            }
        }
    }

}
