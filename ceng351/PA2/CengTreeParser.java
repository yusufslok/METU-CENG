import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.io.IOException;
import java.util.ArrayList;


public class CengTreeParser
{
    public static ArrayList<CengBook> parseBooksFromFile(String filename)
    {
        ArrayList<CengBook> bookList = new ArrayList<CengBook>();

        try{
            File input = new File(filename);
            Scanner scan = new Scanner(input);
            while(scan.hasNextLine()){
                String l = scan.nextLine();
                String[] elements = l.split("[|]");
                Integer key = Integer.parseInt(elements[0]);
                String elem1 = elements[1];
                String elem2 = elements[2];
                String elem3 = elements[3];

                CengBook temp = new CengBook(key, elem1, elem2, elem3);
                bookList.add(temp);
            }
        }
        catch (FileNotFoundException e){
            e.printStackTrace();
        }

        return bookList;
    }

    public static void startParsingCommandLine() throws IOException
    {
        Scanner scan = new Scanner(System.in);
        while(scan.hasNextLine()){
            String l = scan.nextLine();
            String[] elements = l.split("\\|");
            String first = elements[0];
            if(first.equals("quit")) break;
            else if(first.equals("add")){
                Integer key = Integer.parseInt(elements[1]);
                String elem1 = elements[2];
                String elem2 = elements[3];
                String elem3 = elements[4];

                CengBook temp = new CengBook(key, elem1, elem2, elem3);
                CengBookRunner.addBook(temp);
            }
            else if(first.equals("search")){
                Integer key = Integer.parseInt(elements[1]);
                CengBookRunner.searchBook(key);
            }
            else if(first.equals("print")){
                CengBookRunner.printTree();
            }

        }
    }
}


