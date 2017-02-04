class DataHandler{
  Table table;
  String filename;
  
  public DataHandler(String filename){
    this.filename = filename;  
    readInput();
  }
  
  public void tableInfo(){
    println(table.getRowCount() + " rows in table"); 
    println(table.getColumnCount() + " number of attributes"); 
  }
  
  public int getDimension(){
    return table.getColumnCount() ;
  }
  
  public ArrayList<String> getColumnNames(){
    Table tb = loadTable(filename,"csv");
    ArrayList<String> attributes = new ArrayList<String>();
    TableRow tr = tb.getRow(0);
    for(int i=0;i<getDimension();i++){
      attributes.add(tr.getString(i));
    }
    return attributes;
  }
  
  public float getMax(String colName){
    ArrayList<Float> col = new ArrayList<Float>();
    Float max = -1000000.0;
    for (TableRow row : table.rows()) {
      Float val = row.getFloat(colName);
      if(val>max){
        max=val; 
      }
    }
    return max;
  }
  
  public float getMin(String colName){
    ArrayList<Float> col = new ArrayList<Float>();
    Float min = 1000000.0;
    for (TableRow row : table.rows()) {
      Float val = row.getFloat(colName);
      if(val<min){
        min=val; 
      }
    }
    return min;
  }
  
  void readInput(){
    table = loadTable(filename, "header, csv");
  }
  
}