package ceng.ceng351.cengfactorydb;
import java.sql.*;
import java.lang.*;
import java.util.*;

public class CENGFACTORYDB implements ICENGFACTORYDB{
    /**
     * Place your initialization code inside if required.
     *
     *
     * <p>
     * This function will be called before all other operations. If your implementation
     * need initialization , necessary operations should be done inside this function. For
     * example, you can set your connection to the database server inside this function.
     */

    private static String user = "e2521748"; // TODO: Your userName
    private static String password = "2~eYdSNjQTC1"; //  TODO: Your password
    private static String host = "144.122.71.128"; // host name
    private static String database = "db2521748"; // TODO: Your database name
    private static int port = 8080; // port

    private static Connection connection = null;

    public void initialize() {
        try{
            Class.forName("com.mysql.cj.jdbc.Driver");
            connection = DriverManager.getConnection(
                    "jdbc:mysql://"+this.host+":"+this.port+"/"+this.database+"?useSSL=false",this.user,this.password);

        }
        catch (SQLException | ClassNotFoundException e){
            e.printStackTrace();
        }
    }


    /**
     * Should create the necessary tables when called.
     *
     * @return the number of tables that are created successfully.
     */
    public int createTables() {

        int numberOfTables=0;

        String createQueryFactory = "CREATE TABLE IF NOT EXISTS Factory("+
                "factoryId INT, "+
                "factoryName Text, "+
                "factoryType Text, "+
                "country Text, "+
                "PRIMARY KEY (factoryId));";
        String createQueryEmployee = "CREATE TABLE IF NOT EXISTS Employee("+
                "employeeId INT, "+
                "employeeName Text, "+
                "department Text, "+
                "salary INT, "+
                "PRIMARY KEY (employeeId));";
        String createQueryWorks_In = "CREATE TABLE IF NOT EXISTS Works_In("+
                "factoryId INT, "+
                "employeeId INT, "+
                "startDate Date, "+
                "PRIMARY KEY (factoryId, employeeId), "+
                "FOREIGN KEY (factoryId) REFERENCES Factory(factoryId), "+
                "FOREIGN KEY (employeeId) REFERENCES Employee(employeeId) ON DELETE CASCADE);";
        String createQueryProduct = "CREATE TABLE IF NOT EXISTS Product("+
                "productId INT, "+
                "productName Text, "+
                "productType Text, "+
                "PRIMARY KEY (productId));";
        String createQueryProduce = "CREATE TABLE IF NOT EXISTS Produce("+
                "factoryId INT, "+
                "productId INT, "+
                "amount INT, "+
                "productionCost INT, "+
                "PRIMARY KEY (factoryId,productId), "+
                "FOREIGN KEY (factoryId) REFERENCES Factory(factoryId), "+
                "FOREIGN KEY (productId) REFERENCES Product(productId));";
        String createQueryShipment = "CREATE TABLE IF NOT EXISTS Shipment("+
                "factoryId INT, "+
                "productId INT, "+
                "amount INT, "+
                "pricePerUnit INT, "+
                "PRIMARY KEY (factoryId,productId), "+
                "FOREIGN KEY (factoryId) REFERENCES Factory(factoryId), "+
                "FOREIGN KEY (productId) REFERENCES Product(productId));";
        try{
            Statement stmt = connection.createStatement();
            stmt.executeUpdate(createQueryFactory);
            numberOfTables++;
            stmt.close();
        }
        catch (SQLException e){
            e.printStackTrace();
        }

        try{
            Statement stmt = connection.createStatement();
            stmt.executeUpdate(createQueryEmployee);
            numberOfTables++;
            stmt.close();
        }
        catch (SQLException e){
            e.printStackTrace();
        }

        try{
            Statement stmt = connection.createStatement();
            stmt.executeUpdate(createQueryWorks_In);
            numberOfTables++;
            stmt.close();
        }
        catch (SQLException e){
            e.printStackTrace();
        }


        try{
            Statement stmt = connection.createStatement();
            stmt.executeUpdate(createQueryProduct);
            numberOfTables++;
            stmt.close();
        }
        catch (SQLException e){
            e.printStackTrace();
        }

        try{
            Statement stmt = connection.createStatement();
            stmt.executeUpdate(createQueryProduce);
            numberOfTables++;
            stmt.close();
        }
        catch (SQLException e){
            e.printStackTrace();
        }


        try{
            Statement stmt = connection.createStatement();
            stmt.executeUpdate(createQueryShipment);
            numberOfTables++;
            stmt.close();
        }
        catch (SQLException e){
            e.printStackTrace();
        }

        return numberOfTables;
    }


    /**
     * Should drop the tables if exists when called.
     *
     * @return the number of tables are dropped successfully.
     */
    public int dropTables() {
        int number_of_tables_drop=0;

        String[] dropList = new String[]{
                "Shipment",
                "Produce",
                "Product",
                "Works_In",
                "Employee",
                "Factory"
        };

        for(String table : dropList){
            try{
                String dropString = "DROP TABLE IF EXISTS " + table + ";";
                Statement stmt = connection.createStatement();
                stmt.executeUpdate(dropString);
                number_of_tables_drop++;
                stmt.close();
            }
            catch (SQLException e){
                e.printStackTrace();
            }
        }

        return number_of_tables_drop;
    }

    /**
     * Should insert an array of Factory into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertFactory(Factory[] factories) {
        int numberOfInsert=0;

        for(Factory fact : factories){
            try{
                String str_ins_fac = "INSERT INTO Factory(factoryId, factoryName, factoryType, country) VALUES (?,?,?,?);";
                PreparedStatement stmt = connection.prepareStatement(str_ins_fac);
                stmt.setInt(1, fact.getFactoryId());
                stmt.setString(2, fact.getFactoryName());
                stmt.setString(3, fact.getFactoryType());
                stmt.setString(4, fact.getCountry());
                stmt.executeUpdate();
                numberOfInsert++;
                stmt.close();
            }
            catch (SQLException e){
                e.printStackTrace();
            }
        }
        return numberOfInsert;
    }

    /**
     * Should insert an array of Employee into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertEmployee(Employee[] employees) {

        int numberOfInsert=0;

        for(Employee emp : employees){
            try{
                String str_ins_emp = "INSERT INTO Employee(employeeId, employeeName, department, salary) VALUES (?,?,?,?);";
                PreparedStatement stmt = connection.prepareStatement(str_ins_emp);
                stmt.setInt(1, emp.getEmployeeId());
                stmt.setString(2, emp.getEmployeeName());
                stmt.setString(3, emp.getDepartment());
                stmt.setInt(4, emp.getSalary());
                stmt.executeUpdate();
                numberOfInsert++;
                stmt.close();
            }
            catch (SQLException e){
                e.printStackTrace();
            }
        }
        return numberOfInsert;
    }

    /**
     * Should insert an array of WorksIn into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertWorksIn(WorksIn[] worksIns) {
        int numberOfInsert=0;

        for(WorksIn work : worksIns){
            try{
                String str_ins_work = "INSERT INTO Works_In(factoryId, employeeId, startDate) VALUES (?,?,?);";
                PreparedStatement stmt = connection.prepareStatement(str_ins_work);
                stmt.setInt(1, work.getFactoryId());
                stmt.setInt(2, work.getEmployeeId());
                stmt.setString(3, work.getStartDate());
                stmt.executeUpdate();
                numberOfInsert++;
                stmt.close();
            }
            catch (SQLException e){
                e.printStackTrace();
            }
        }
        return numberOfInsert;
    }

    /**
     * Should insert an array of Product into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertProduct(Product[] products) {
        int numberOfInsert=0;

        for(Product product : products){
            try{
                String str_ins_product = "INSERT INTO Product(productId, productName, productType) VALUES (?,?,?);";
                PreparedStatement stmt = connection.prepareStatement(str_ins_product);
                stmt.setInt(1, product.getProductId());
                stmt.setString(2, product.getProductName());
                stmt.setString(3, product.getProductType());
                stmt.executeUpdate();
                numberOfInsert++;
                stmt.close();
            }
            catch (SQLException e){
                e.printStackTrace();
            }
        }
        return numberOfInsert;
    }


    /**
     * Should insert an array of Produce into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertProduce(Produce[] produces) {
        int numberOfInsert=0;

        for(Produce produce : produces){
            try{
                String str_ins_produce = "INSERT INTO Produce(factoryId, productId, amount, productionCost) VALUES (?,?,?,?);";
                PreparedStatement stmt = connection.prepareStatement(str_ins_produce);
                stmt.setInt(1, produce.getFactoryId());
                stmt.setInt(2, produce.getProductId());
                stmt.setInt(3, produce.getAmount());
                stmt.setInt(4, produce.getProductionCost());
                stmt.executeUpdate();
                numberOfInsert++;
                stmt.close();
            }
            catch (SQLException e){
                e.printStackTrace();
            }
        }
        return numberOfInsert;
    }


    /**
     * Should insert an array of Shipment into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertShipment(Shipment[] shipments) {
        int numberOfInsert=0;

        for(Shipment shipment : shipments){
            try{
                String str_ins_shipment = "INSERT INTO Shipment(factoryId, productId, amount, pricePerUnit) VALUES (?,?,?,?);";
                PreparedStatement stmt = connection.prepareStatement(str_ins_shipment);
                stmt.setInt(1, shipment.getFactoryId());
                stmt.setInt(2, shipment.getProductId());
                stmt.setInt(3, shipment.getAmount());
                stmt.setInt(4, shipment.getPricePerUnit());
                stmt.executeUpdate();
                numberOfInsert++;
                stmt.close();
            }
            catch (SQLException e){
                e.printStackTrace();
            }
        }
        return numberOfInsert;
    }

    /**
     * Should return all factories that are located in a particular country.
     *
     * @return Factory[]
     */
    public Factory[] getFactoriesForGivenCountry(String country) {
        List<Factory> factoryList = new ArrayList<>();

        String getFactForGivenCountry = "SELECT F.factoryId, F.factoryName, F.factoryType, F.country " +
                "FROM Factory F " +
                "WHERE F.country = ? " +
                "ORDER BY factoryId";

        try (PreparedStatement stmt = connection.prepareStatement(getFactForGivenCountry)) {
            stmt.setString(1, country);
            ResultSet result = stmt.executeQuery();

            while (result.next()) {
                int factoryId = result.getInt("factoryId");
                String factoryName = result.getString("factoryName");
                String factoryType = result.getString("factoryType");
                Factory fact = new Factory(factoryId, factoryName, factoryType, country);
                factoryList.add(fact);
            }

        } catch (SQLException e) {
            e.printStackTrace();
        }

        // Convert the ArrayList to an array of Factory
        Factory[] factoryArray = factoryList.toArray(new Factory[0]);

        return factoryArray;
    }



    /**
     * Should return all factories without any working employees.
     *
     * @return Factory[]
     */
    public Factory[] getFactoriesWithoutAnyEmployee() {
        List<Factory> factoryList = new ArrayList<>();

        String getFactForGivenCountry = "SELECT F.factoryId, F.factoryName, F.factoryType, F.country " +
                "FROM Factory F " +
                "WHERE NOT EXISTS (SELECT * FROM Works_In W WHERE W.factoryId = F.factoryId) " +
                "ORDER BY factoryId";

        try (PreparedStatement stmt = connection.prepareStatement(getFactForGivenCountry)) {
            ResultSet result = stmt.executeQuery();

            while (result.next()) {
                int factoryId = result.getInt("factoryId");
                String factoryName = result.getString("factoryName");
                String factoryType = result.getString("factoryType");
                String country = result.getString("country");
                Factory fact = new Factory(factoryId, factoryName, factoryType, country);
                factoryList.add(fact);
            }

        } catch (SQLException e) {
            e.printStackTrace();
        }

        // Convert the ArrayList to an array of Factory
        Factory[] factoryArray = factoryList.toArray(new Factory[0]);

        return factoryArray;
    }

    /**
     * Should return all factories that produce all products for a particular productType
     *
     * @return Factory[]
     */
    public Factory[] getFactoriesProducingAllForGivenType(String productType) {
        List<Factory> factoryList = new ArrayList<>();

        String getFactForGivenCountry = "SELECT F.factoryId, F.factoryName, F.factoryType, F.country " +
                "FROM Factory F " +
                "WHERE NOT EXISTS(" +
                "SELECT P.productId "+
                "FROM Product P "+
                "WHERE P.productType = ? "+
                "EXCEPT "+
                "SELECT P.productId "+
                "FROM Produce P "+
                "WHERE P.factoryId = F.factoryId) "+
                "ORDER BY factoryId";

        try (PreparedStatement stmt = connection.prepareStatement(getFactForGivenCountry)) {
            stmt.setString(1, productType);
            ResultSet result = stmt.executeQuery();

            while (result.next()) {
                int factoryId = result.getInt("factoryId");
                String factoryName = result.getString("factoryName");
                String factoryType = result.getString("factoryType");
                String country = result.getString("country");
                Factory fact = new Factory(factoryId, factoryName, factoryType, country);
                factoryList.add(fact);
            }

        } catch (SQLException e) {
            e.printStackTrace();
        }

        // Convert the ArrayList to an array of Factory
        Factory[] factoryArray = factoryList.toArray(new Factory[0]);

        return factoryArray;
    }


    /**
     * Should return the products that are produced in a particular factory but
     * don’t have any shipment from that factory.
     *
     * @return Product[]
     */
    public Product[] getProductsProducedNotShipped() {
        List<Product> factoryList = new ArrayList<>();

        String getFactForGivenCountry = "SELECT DISTINCT P.productId, P.productName, P.productType " +
                "FROM Factory F, Produce P1, Product P "+
                "WHERE F.factoryId = P1.factoryId AND P1.productId = P.productId AND  NOT EXISTS( "+
                "SELECT * "+
                "FROM Shipment S "+
                "WHERE F.factoryId = S.factoryId AND S.productId = P.productId) "+
                "ORDER BY P.productId";


        try (PreparedStatement stmt = connection.prepareStatement(getFactForGivenCountry)) {
            ResultSet result = stmt.executeQuery();

            while (result.next()) {
                int productId = result.getInt("productId");
                String productName = result.getString("productName");
                String productType = result.getString("productType");
                Product fact = new Product(productId, productName, productType);
                factoryList.add(fact);
            }

        } catch (SQLException e) {
            e.printStackTrace();
        }

        // Convert the ArrayList to an array of Factory
        Product[] factoryArray = factoryList.toArray(new Product[0]);

        return factoryArray;
    }


    /**
     * For a given factoryId and department, should return the average salary of
     *     the employees working in that factory and that specific department.
     *
     * @return double
     */
    public double getAverageSalaryForFactoryDepartment(int factoryId, String department) {
        double avg=0;
        String getFactForGivenCountry = "SELECT AVG(E.salary) as avg "+
                "FROM Employee E, Works_In W "+
                "WHERE E.employeeId = W.employeeId AND E.department = ? AND W.factoryId = ? ";


        try (PreparedStatement stmt = connection.prepareStatement(getFactForGivenCountry)) {
            stmt.setString(1, department);
            stmt.setInt(2, factoryId);
            try (ResultSet result = stmt.executeQuery()) {
                if (result.next()) {
                    avg = result.getDouble("avg");
                }
            }

        } catch (SQLException e) {
            e.printStackTrace();
        }


        return avg;
    }


    /**
     * Should return the most profitable products for each factory
     *
     * @return QueryResult.MostValueableProduct[]
     */
    public QueryResult.MostValueableProduct[] getMostValueableProducts() {
        List<QueryResult.MostValueableProduct> queryList = new ArrayList<>();
        String str = "SELECT temp3.factoryId, temp3.productId, temp3.productName, temp3.productType, temp3.profiT "+
                "FROM ("+
                "SELECT temp1.factoryId, temp1.productId, temp1.productName, temp1.productType, temp1.profiT "+
                "FROM "+
                "(SELECT temp.factoryId, temp.productId, temp.productName, temp.productType, temp.profit "+
                "FROM "+
                "(SELECT P.factoryId, T.productId, T.productName, T.productType, S.amount*S.pricePerUnit-P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.factoryId = S.factoryId AND P.productId = T.productId AND S.productId = P.productId) AS temp "+
                "WHERE temp.profit = (SELECT MAX(tempp.profit) "+
                "FROM "+
                "(SELECT P.factoryId AS factoryId, T.productId, T.productName, T.productType, S.amount*S.pricePerUnit-P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.factoryId = S.factoryId AND P.productId = T.productId AND S.productId = P.productId) AS tempp "+
                "WHERE tempp.factoryId = temp.factoryId "+
                "GROUP BY tempp.factoryId) "+
                "UNION "+
                "SELECT temp.factoryId, temp.productId, temp.productName, temp.productType, temp.profit "+
                "FROM "+
                "(SELECT P.factoryId, T.productId, T.productName, T.productType, -P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.productId = T.productId AND NOT EXISTS (SELECT * FROM Shipment S WHERE P.factoryId = S.factoryId AND P.productId = S.productId)) AS temp "+
                "WHERE temp.profit = (SELECT MAX(tempp.profit) "+
                "FROM "+
                "(SELECT P.factoryId AS factoryId, T.productId, T.productName, T.productType, -P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.productId = T.productId AND NOT EXISTS (SELECT * FROM Shipment S WHERE P.factoryId = S.factoryId AND P.productId = S.productId)) AS tempp "+
                "WHERE tempp.factoryId = temp.factoryId "+
                "GROUP BY tempp.factoryId)) AS temp1) AS temp3 "+
                "WHERE temp3.profit = (SELECT MAX(temp4.profit) "+
                "FROM ("+

                "SELECT temp1.factoryId, temp1.productId, temp1.productName, temp1.productType, temp1.profiT "+
                "FROM "+
                "(SELECT temp.factoryId, temp.productId, temp.productName, temp.productType, temp.profit "+
                "FROM "+
                "(SELECT P.factoryId, T.productId, T.productName, T.productType, S.amount*S.pricePerUnit-P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.factoryId = S.factoryId AND P.productId = T.productId AND S.productId = P.productId) AS temp "+
                "WHERE temp.profit = (SELECT MAX(tempp.profit) "+
                "FROM "+
                "(SELECT P.factoryId AS factoryId, T.productId, T.productName, T.productType, S.amount*S.pricePerUnit-P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.factoryId = S.factoryId AND P.productId = T.productId AND S.productId = P.productId) AS tempp "+
                "WHERE tempp.factoryId = temp.factoryId "+
                "GROUP BY tempp.factoryId) "+
                "UNION "+
                "SELECT temp.factoryId, temp.productId, temp.productName, temp.productType, temp.profit "+
                "FROM "+
                "(SELECT P.factoryId, T.productId, T.productName, T.productType, -P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.productId = T.productId AND NOT EXISTS (SELECT * FROM Shipment S WHERE P.factoryId = S.factoryId AND P.productId = S.productId)) AS temp "+
                "WHERE temp.profit = (SELECT MAX(tempp.profit) "+
                "FROM "+
                "(SELECT P.factoryId AS factoryId, T.productId, T.productName, T.productType, -P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.productId = T.productId AND NOT EXISTS (SELECT * FROM Shipment S WHERE P.factoryId = S.factoryId AND P.productId = S.productId)) AS tempp "+
                "WHERE tempp.factoryId = temp.factoryId "+
                "GROUP BY tempp.factoryId)) AS temp1) AS temp4 "+
                "WHERE temp3.factoryId = temp4.factoryId) "+
                "ORDER BY profit DESC, factoryId ASC";


        try (PreparedStatement stmt = connection.prepareStatement(str)) {
            ResultSet result = stmt.executeQuery();

            while (result.next()) {
                int factoryId = result.getInt("factoryId");
                int productId = result.getInt("productId");
                String productName = result.getString("productName");
                String productType = result.getString("productType");
                int profit = result.getInt("profit");
                QueryResult.MostValueableProduct fact = new QueryResult.MostValueableProduct(factoryId, productId, productName, productType, profit);
                queryList.add(fact);
            }

        } catch (SQLException e) {
            e.printStackTrace();
        }

        QueryResult.MostValueableProduct[] factoryArray = queryList.toArray(new QueryResult.MostValueableProduct[0]);

        return factoryArray;


    }

    /**
     * For each product, return the factories that gather the highest profit
     * for that product
     *
     * @return QueryResult.MostValueableProduct[]
     */
    public QueryResult.MostValueableProduct[] getMostValueableProductsOnFactory() {
        List<QueryResult.MostValueableProduct> queryList = new ArrayList<>();
        String str =  "SELECT temp3.factoryId, temp3.productId, temp3.productName, temp3.productType, temp3.profiT "+
                "FROM ("+
                "SELECT temp1.factoryId, temp1.productId, temp1.productName, temp1.productType, temp1.profiT "+
                "FROM "+
                "(SELECT temp.factoryId, temp.productId, temp.productName, temp.productType, temp.profit "+
                "FROM "+
                "(SELECT P.factoryId, T.productId, T.productName, T.productType, S.amount*S.pricePerUnit-P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.factoryId = S.factoryId AND P.productId = T.productId AND S.productId = P.productId) AS temp "+
                "WHERE temp.profit = (SELECT MAX(tempp.profit) "+
                "FROM "+
                "(SELECT P.factoryId AS factoryId, T.productId, T.productName, T.productType, S.amount*S.pricePerUnit-P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.factoryId = S.factoryId AND P.productId = T.productId AND S.productId = P.productId) AS tempp "+
                "WHERE tempp.productId = temp.productId "+
                "GROUP BY tempp.productId) "+
                "UNION "+
                "SELECT temp.factoryId, temp.productId, temp.productName, temp.productType, temp.profit "+
                "FROM "+
                "(SELECT P.factoryId, T.productId, T.productName, T.productType, -P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.productId = T.productId AND NOT EXISTS (SELECT * FROM Shipment S WHERE P.factoryId = S.factoryId AND P.productId = S.productId)) AS temp "+
                "WHERE temp.profit = (SELECT MAX(tempp.profit) "+
                "FROM "+
                "(SELECT P.factoryId AS factoryId, T.productId, T.productName, T.productType, -P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.productId = T.productId AND NOT EXISTS (SELECT * FROM Shipment S WHERE P.factoryId = S.factoryId AND P.productId = S.productId)) AS tempp "+
                "WHERE tempp.productId = temp.productId "+
                "GROUP BY tempp.productId)) AS temp1) AS temp3 "+
                "WHERE temp3.profit = (SELECT MAX(temp4.profit) "+
                "FROM ("+

                "SELECT temp1.factoryId, temp1.productId, temp1.productName, temp1.productType, temp1.profiT "+
                "FROM "+
                "(SELECT temp.factoryId, temp.productId, temp.productName, temp.productType, temp.profit "+
                "FROM "+
                "(SELECT P.factoryId, T.productId, T.productName, T.productType, S.amount*S.pricePerUnit-P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.factoryId = S.factoryId AND P.productId = T.productId AND S.productId = P.productId) AS temp "+
                "WHERE temp.profit = (SELECT MAX(tempp.profit) "+
                "FROM "+
                "(SELECT P.factoryId AS factoryId, T.productId, T.productName, T.productType, S.amount*S.pricePerUnit-P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.factoryId = S.factoryId AND P.productId = T.productId AND S.productId = P.productId) AS tempp "+
                "WHERE tempp.productId = temp.productId "+
                "GROUP BY tempp.productId) "+
                "UNION "+
                "SELECT temp.factoryId, temp.productId, temp.productName, temp.productType, temp.profit "+
                "FROM "+
                "(SELECT P.factoryId, T.productId, T.productName, T.productType, -P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.productId = T.productId AND NOT EXISTS (SELECT * FROM Shipment S WHERE P.factoryId = S.factoryId AND P.productId = S.productId)) AS temp "+
                "WHERE temp.profit = (SELECT MAX(tempp.profit) "+
                "FROM "+
                "(SELECT P.factoryId AS factoryId, T.productId, T.productName, T.productType, -P.amount*P.productionCost AS profit "+
                "FROM Produce P, Shipment S, Product T "+
                "WHERE P.productId = T.productId AND NOT EXISTS (SELECT * FROM Shipment S WHERE P.factoryId = S.factoryId AND P.productId = S.productId)) AS tempp "+
                "WHERE tempp.productId = temp.productId "+
                "GROUP BY tempp.productId)) AS temp1) AS temp4 "+
                "WHERE temp3.productId = temp4.productId) "+
                "ORDER BY profit DESC, productId ASC";

        try (PreparedStatement stmt = connection.prepareStatement(str)) {
            ResultSet result = stmt.executeQuery();

            while (result.next()) {
                int factoryId = result.getInt("factoryId");
                int productId = result.getInt("productId");
                String productName = result.getString("productName");
                String productType = result.getString("productType");
                int profit = result.getInt("profit");
                QueryResult.MostValueableProduct fact = new QueryResult.MostValueableProduct(factoryId, productId, productName, productType, profit);
                queryList.add(fact);
            }

        } catch (SQLException e) {
            e.printStackTrace();
        }

        QueryResult.MostValueableProduct[] factoryArray = queryList.toArray(new QueryResult.MostValueableProduct[0]);

        return factoryArray;
    }


    /**
     * For each department, should return all employees that are paid under the
     *     average salary for that department. You consider the employees
     *     that do not work earning ”0”.
     *
     * @return QueryResult.LowSalaryEmployees[]
     */
    public QueryResult.LowSalaryEmployees[] getLowSalaryEmployeesForDepartments() {
        List<QueryResult.LowSalaryEmployees> queryList = new ArrayList<>();

        String str = "(SELECT E.employeeId, E.employeeName, E.department, E.salary " +
                "FROM Employee E, Works_In W " +
                "WHERE E.employeeId = W.employeeId AND E.salary < (" +
                "    SELECT SUM(E2.salary)/COUNT(E1.salary) " +
                "    FROM Employee E2, Employee E1, Works_In W1 " +
                "    WHERE E2.employeeId = W1.employeeId AND E2.department = E.department) " +
                "UNION " +
                "SELECT E.employeeId, E.employeeName, E.department, 0 AS salary " +
                "FROM Employee E " +
                "WHERE NOT EXISTS (" +
                "    SELECT * " +
                "    FROM Works_In W " +
                "    WHERE E.employeeId = W.employeeId) AND EXISTS (SELECT * FROM Employee E2 WHERE E.department = E2.department AND E2.salary != 0)) " +
                "ORDER BY employeeId ASC";


        try (PreparedStatement stmt = connection.prepareStatement(str)) {
            ResultSet result = stmt.executeQuery();

            while (result.next()) {
                int employeeId = result.getInt("employeeId");
                String employeeName = result.getString("employeeName");
                String department = result.getString("department");
                int salary = result.getInt("salary");
                QueryResult.LowSalaryEmployees fact = new QueryResult.LowSalaryEmployees(employeeId, employeeName, department, salary);
                queryList.add(fact);
            }

        } catch (SQLException e) {
            e.printStackTrace();
        }

        QueryResult.LowSalaryEmployees[] factoryArray = queryList.toArray(new QueryResult.LowSalaryEmployees[0]);

        return factoryArray;
    }





    /**
     * For the products of given productType, increase the productionCost of every unit by a given percentage.
     *
     * @return number of rows affected
     */
    public int increaseCost(String productType, double percentage) {
        String str = "UPDATE Produce " +
                "SET productionCost = productionCost + ? * productionCost " +
                "WHERE productId IN("+
                "SELECT P.productId "+
                "FROM Product P "+
                "WHERE P.productType = ?)";
        int result = 0;

        try (PreparedStatement stmt = connection.prepareStatement(str)) {
            stmt.setDouble(1, percentage);
            stmt.setString(2, productType);
            result = stmt.executeUpdate();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return result;
    }



    /**
     * Deleting all employees that have not worked since the given date.
     *
     * @return number of rows affected
     */
    public int deleteNotWorkingEmployees(String givenDate) {
        int rowsAffected = 0;

        String deleteQuery = "DELETE FROM Employee WHERE employeeId NOT IN (" +
                "    SELECT employeeId FROM Works_In WHERE startDate >= ?" +
                ")";

        try (PreparedStatement stmt = connection.prepareStatement(deleteQuery)) {
            stmt.setString(1, givenDate);
            rowsAffected = stmt.executeUpdate();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return rowsAffected;
    }



    /**
     * *****************************************************
     * *****************************************************
     * *****************************************************
     * *****************************************************
     *  THE METHODS AFTER THIS LINE WILL NOT BE GRADED.
     *  YOU DON'T HAVE TO SOLVE THEM, LEAVE THEM AS IS IF YOU WANT.
     *  IF YOU HAVE ANY QUESTIONS, REACH ME VIA EMAIL.
     * *****************************************************
     * *****************************************************
     * *****************************************************
     * *****************************************************
     */

    /**
     * For each department, find the rank of the employees in terms of
     * salary. Peers are considered tied and receive the same rank. After
     * that, there should be a gap.
     *
     * @return QueryResult.EmployeeRank[]
     */
    public QueryResult.EmployeeRank[] calculateRank() {
        return new QueryResult.EmployeeRank[0];
    }

    /**
     * For each department, find the rank of the employees in terms of
     * salary. Everything is the same but after ties, there should be no
     * gap.
     *
     * @return QueryResult.EmployeeRank[]
     */
    public QueryResult.EmployeeRank[] calculateRank2() {
        return new QueryResult.EmployeeRank[0];
    }

    /**
     * For each factory, calculate the most profitable 4th product.
     *
     * @return QueryResult.FactoryProfit
     */
    public QueryResult.FactoryProfit calculateFourth() {
        return new QueryResult.FactoryProfit(0,0,0);
    }

    /**
     * Determine the salary variance between an employee and another
     * one who began working immediately after the first employee (by
     * startDate), for all employees.
     *
     * @return QueryResult.SalaryVariant[]
     */
    public QueryResult.SalaryVariant[] calculateVariance() {
        return new QueryResult.SalaryVariant[0];
    }

    /**
     * Create a method that is called once and whenever a Product starts
     * losing money, deletes it from Produce table
     *
     * @return void
     */
    public void deleteLosing() {

    }
}
