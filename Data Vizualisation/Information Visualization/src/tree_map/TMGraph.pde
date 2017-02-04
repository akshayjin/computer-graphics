class TMGraph{
 Rectangle root;
 DataHandler dh;
 public ArrayList<String>attributes;
 
 public TMGraph(String filename){
   dh = new DataHandler(filename);
   dh.tableInfo();
   root = new Rectangle(50,50,1850,1850,"Cars",false,0,dh);
   attributes = dh.getColumnNames();
 }
 
 
 
 public void drawGraph(){
    root.drawRectangle(dh);

 }
  
}