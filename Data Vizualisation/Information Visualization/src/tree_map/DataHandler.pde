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
  
  void readInput(){
    table = loadTable(filename, "header, csv");
  }
  
  public HashMap<String,Integer> attributeValues(String attribute,String pastVals,int level){
    String[] temp = splitTokens(pastVals,".");
    HashMap<String,Integer> hm = new HashMap<String,Integer>();
    for (TableRow row : table.rows()) {
      String val = row.getString(attribute);
      boolean checkPast = true;
      for(int i=0;i<level;i++){
        if(temp[i+1].compareTo(row.getString(i))!=0){
          checkPast = false;
          break;
        }
      }
      if(checkPast){
        if(hm.containsKey(val)){
          hm.put(val,hm.get(val) + 1);
        }
        else{
          hm.put(val,1);
        }
      }
    }
    return hm;
  }
  
  public String findClass(String[] attributeValues){
     for (TableRow row : table.rows()) {
       boolean check = true;
       for(int i=0;i<getDimension()-1;i++){
          if(attributeValues[i].compareTo(row.getString(i))!=0){
            check=false;
            break;
          }
       }
       if (check)
         return row.getString("class");
     }
     return "u";
  }
}