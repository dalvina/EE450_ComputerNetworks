import java.io.IOException;
import java.util.stream.Stream;
import java.util.Scanner;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;
import java.lang.*;

public class STDM {

    public static void main(String args[]) throws Exception {
        int sourceNum = 0;
        ArrayList<String> dataList = new ArrayList<String>();
        String filename = args[0];
        java.io.File file = new java.io.File(filename);
        Scanner in = new Scanner(file);
        
        //import all data into an Arraylist;
        while (in.hasNext()) {
            dataList.add(in.nextLine());
        }
        sourceNum = dataList.size();
        in.close();

        ArrayList<String> dataList1 = new ArrayList<String>();


        // Parse string and store in the dataList1 ;
        for (int i = 0; i < dataList.size(); i++) {
            String str = dataList.get(i);
            int x = str.indexOf(" ");
            str = str.substring(x);
            dataList1.add(str);
        }

        for (int i = 0; i < dataList1.size(); i++) {
            System.out.println(dataList1.get(i));
        }

        ArrayList<String[]> data = new ArrayList<String[]>();
        
        for (int i = 0; i < dataList1.size(); i++) {
            data.add(dataList1.get(i).split("\\,"));

        }
        //traverse the datalist to get slotNum and num of subframes
        int slotNum = 0;
        int subNum = 0;
        int count = 0;
        for (int i = 0; i < data.size(); i++) {
            for (int j = 0; j < data.get(i).length; j++) {
                if(slotNum<= Integer.parseInt(data.get(i)[j].substring(3,4))) {
                	slotNum = Integer.parseInt(data.get(i)[j].substring(3,4));
                }
                count++;
            }
        }
        
        subNum = (int)Math.ceil(((double)count/slotNum));
        ArrayList<String> dataList2 = new ArrayList<String>();
        for (int i = 0; i < data.size(); i++) {
            for (int j = 0; j < data.get(i).length; j++) {

                String buffer = data.get(i)[j];
                buffer = buffer.substring(3,4)+" "+toBinary(i) + buffer.substring(5);
                dataList2.add(buffer);
            }
        }
        Collections.sort(dataList2);

        // Put all the data in the OutputBUffer
        Queue<String>bufferOut = new LinkedList<String>();

        for (int i = 0; i < dataList2.size(); i++) {
        	bufferOut.offer(dataList2.get(i));
        }
        
        // Print out all start time, end time and frames
        String startFlag = "SF", endFlag = "EF";
        StringBuilder frame = new StringBuilder("");
        String buffer;
        for(int i = 1; i<= slotNum; i++){
           int num=1;
           buffer = "";
           while(bufferOut.peek()!= null && Integer.parseInt(bufferOut.peek().substring(0,1))<= i  ){
        	   buffer=buffer+bufferOut.poll().substring(2);
        	   num++;
        	   if(num > subNum) {
                    break;
        	   }
           }
           frame.append(i-1+" ");
           frame.append(startFlag);
           frame.append(buffer);
           frame.append(endFlag+" ");
           frame.append(i+ "    ");
   
       }
        System.out.println( frame.toString());

    }
    //Convert a decimal into binary representation 
    public static String toBinary(int n) {
      String binaryNum = Integer.toBinaryString(n);
      if(binaryNum.equals("0")){
          return "00";
      }
      if(binaryNum.equals("1")){
          return "01";
      }
      return binaryNum;
   }

}




