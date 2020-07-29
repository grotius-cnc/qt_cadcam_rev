/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package grotius;

//java -cp lib/nest4J-0.0.1-jar-with-dependencies.jar lib/dom4j-2.0.3.jar grotius_nest_interface
import com.qunhe.util.nest.Nest;
import com.qunhe.util.nest.data.*;
import com.qunhe.util.nest.util.Config;
import com.qunhe.util.nest.util.GeometryUtil;
import com.qunhe.util.nest.util.PostionUtil;
import com.qunhe.util.nest.util.SvgUtil;
import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.Element;
import org.dom4j.io.SAXReader;
import org.junit.Test;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;
import java.util.ArrayList;
import java.util.List;

import java.util.*;
import java.util.logging.Level;
import java.util.logging.Logger;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

//alt-shift-f for auto intent.
/**
 *
 * @author Grotius Date : 15.07.2020 My first Java application
 */
public class grotius_nest_interface {

    public static void main(String[] args) throws IOException {

        // TODO code application logic here
        System.out.println("read in active..");

        ArrayList<String> list_in = new ArrayList<String>();            //input list that holds data from textfile "nest_in.txt"
        List<NestPath> list_to_nest = new ArrayList<NestPath>();        //final polygon list to pas into the nest function.

        try { //read in file line by line and store data in list_in.
            Scanner scanner = new Scanner(new File("nest_in.txt"));
            while (scanner.hasNextLine()) {
                //System.out.println(scanner.nextLine());
                list_in.add(scanner.nextLine());
            }
            scanner.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        //create polygons from list_in, separate xy values, and pass result into final polygon list : list_to_nest.
        //NestPath polygon = new NestPath();
        ArrayList<Double> list_x = new ArrayList<Double>();
        ArrayList<Double> list_y = new ArrayList<Double>();
        ArrayList<Integer> bids_total = new ArrayList<Integer>();
        ArrayList<Integer> bids_done = new ArrayList<Integer>();

        int platenumber=0;
        int id = 0;
        int rot = 0;
        double spacing = 0;
        int mutation_rate = 10;
        int population_size = 10;
        boolean use_hole = true;
        double binstart_x = 500;
        double binstart_y = 0;
        double binwidth = 10000;  //this is the nest area
        double binheight = 10000;

        for (int i = 0; i < list_in.size(); i++) {

            //System.out.println(list_in.get(i));
            if (!list_in.get(i).isEmpty() && !list_in.get(i).contains("end") && !list_in.get(i).contains("bid") && !list_in.get(i).contains("rotation")
                    && !list_in.get(i).contains("binwidth") && !list_in.get(i).contains("binheight") && !list_in.get(i).contains("spacing")
                    && !list_in.get(i).contains("use_hole") && !list_in.get(i).contains("population_size") && !list_in.get(i).contains("mutation_rate")
                    && !list_in.get(i).contains("binstart_x") && !list_in.get(i).contains("binstart_y")) {

                String[] splited = list_in.get(i).split("\\s+");
                double x = 0, y = 0;
                for (int j = 0; j < splited.length; j++) {
                    if (j == 0) {
                        x = Double.parseDouble(splited[j]);
                    }
                    if (j == 1) {
                        y = Double.parseDouble(splited[j]);
                    }
                }
                list_x.add(x);
                list_y.add(y);
            } 
            if (list_in.get(i).contains("bid")) {
                String[] splited = list_in.get(i).split("\\s+");
                for (int j = 0; j < splited.length; j++) {
                    if (j == 1) {
                        id = Integer.valueOf(splited[j]);
                        bids_total.add(id); //list with all the bids / id's.
                    }
                }
            } 
            if (list_in.get(i).contains("end")) { //write data
                NestPath polygon = new NestPath();
                for (int j = 0; j < list_x.size(); j++) {
                    polygon.add(list_x.get(j), list_y.get(j));
                }
                polygon.bid = id;
                polygon.setRotation(rot);
                list_to_nest.add(polygon); //here the polygon is finished and added to the list_to_nest.
                list_x.clear();
                list_y.clear();
            } 
            if (list_in.get(i).contains("spacing")) {
                String[] splited = list_in.get(i).split("\\s+");
                for (int j = 0; j < splited.length; j++) {
                    if (j == 1) {
                        spacing = Double.valueOf(splited[j]);
                    }
                }
            } 
            if (list_in.get(i).contains("use_hole")) {
                String[] splited = list_in.get(i).split("\\s+");
                for (int j = 0; j < splited.length; j++) {
                    if (j == 1) {
                        use_hole = Boolean.valueOf(splited[j]);
                    }
                }
            } 
            if (list_in.get(i).contains("population_size")) {
                String[] splited = list_in.get(i).split("\\s+");
                for (int j = 0; j < splited.length; j++) {
                    if (j == 1) {
                        population_size = Integer.valueOf(splited[j]);
                    }
                }
            } 
            if (list_in.get(i).contains("mutation_rate")) {
                String[] splited = list_in.get(i).split("\\s+");
                for (int j = 0; j < splited.length; j++) {
                    if (j == 1) {
                        mutation_rate = Integer.valueOf(splited[j]);
                    }
                }
            } 
            if (list_in.get(i).contains("rotation")) {
                String[] splited = list_in.get(i).split("\\s+");
                for (int j = 0; j < splited.length; j++) {
                    if (j == 1) {
                        rot = Integer.valueOf(splited[j]);
                    }
                }
            } 
            if (list_in.get(i).contains("binwidth")) {
                String[] splited = list_in.get(i).split("\\s+");
                for (int j = 0; j < splited.length; j++) {
                    if (j == 1) {
                        binwidth = Double.valueOf(splited[j]);
                    }
                }
            } 
            if (list_in.get(i).contains("binheight")) {
                String[] splited = list_in.get(i).split("\\s+");
                for (int j = 0; j < splited.length; j++) {
                    if (j == 1) {
                        binheight = Double.valueOf(splited[j]);
                    }
                }
            } 
            if (list_in.get(i).contains("binstart_x")) {
                String[] splited = list_in.get(i).split("\\s+");
                for (int j = 0; j < splited.length; j++) {
                    if (j == 1) {
                        binstart_x = Double.valueOf(splited[j]);
                    }
                }
            } 
            if (list_in.get(i).contains("binstart_y")) {
                String[] splited = list_in.get(i).split("\\s+");
                for (int j = 0; j < splited.length; j++) {
                    if (j == 1) {
                        binstart_y = Double.valueOf(splited[j]);
                    }
                }
            }

        }

        NestPath nestarea = new NestPath();
        nestarea.add(binstart_x, binstart_y);
        nestarea.add(binstart_x, binstart_y + binheight);
        nestarea.add(binstart_x + binwidth, binstart_y + binheight);
        nestarea.add(binstart_x + binwidth, binstart_y);
        nestarea.add(binstart_x, binstart_y);

        //For those hollow polgyons, Nest4J provides a simple way to express by 2d coordinate system. 
        //If one polygon is inside in another by their coordinates, the Nest4J will detact it automaticly.
        //When one polygon is constructed , its default Rotation attr is 0 , 
        //which means we will fix it during our Nest Program. 
        //We can set it as 4 and this polygon may rotate in 90°,180°,270°. 
        //If we set Rotation attr as N, this polygon may these rotation angles (360/N) *k , k= 0,1,2,3,...N
        //Meanwhile you can use bid to help you identify the polygons. 
        //It is useful when we get nest result.
        //Before we start to nest , you can set configuration.
        //Attr 	Description 	Default
        //SPACING               the distance of each plygons on bin             0
        //POPULATION_SIZE 	the number of population in GA algorithm 	10
        //MUTATION_RATE 	the rate of mutate in GA algorithm              10%
        //USE_HOLE              allow to put polygons into hollow polygons      false
        Config config = new Config();
        config.SPACING = spacing;
        config.USE_HOLE = use_hole;
        config.POPULATION_SIZE = population_size;
        config.MUTATION_RATE = mutation_rate;

        System.out.println("\nstartnest..\n");
        
        
        //open textfile for write data:
          System.out.println("\nstreaming to file..");
        File f0 = new File("nest_out.txt");
        if (!f0.exists()) {
            try {
                f0.createNewFile();
            } catch (IOException ex) {
                Logger.getLogger(grotius_nest_interface.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        Writer writer0 = null;
        try {
            writer0 = new FileWriter(f0, false);
        } catch (IOException ex) {
            Logger.getLogger(grotius_nest_interface.class.getName()).log(Level.SEVERE, null, ex);
        }
        //textfile is active for writing now..
        

        do {
        //When we configure the bin, the material list and the configuration, we can start to nest.
        Nest nest = new Nest(nestarea, list_to_nest, config, 10);
        

        List<List<Placement>> appliedPlacement = nest.startNest();
        ArrayList<String> list_out = new ArrayList<String>();

        for (Placement placement : appliedPlacement.get(0)) {

            String bid = Integer.toString(placement.bid);
            String translation_x = Double.toString(placement.translate.x);
            String translation_y = Double.toString(placement.translate.y);
            String rotation = Double.toString(placement.rotate);
            System.out.println("bid: " + bid + ", platenumber: " + platenumber + ", trans_x: " + translation_x + ", trans_y: " + translation_y + ", rot: " + rotation);
            
            list_out.add("bid");
            list_out.add(bid);
            list_out.add("platenumber");
            list_out.add(Integer.toString(platenumber));
            list_out.add("translation x");
            list_out.add(translation_x);
            list_out.add("translation y");
            list_out.add(translation_y);
            list_out.add("rotation");
            list_out.add(rotation);

            if (bid.contains("-1")) {
                //do nothing
            } else {
                bids_done.add(placement.bid);
            }

        }

        for (Integer i : bids_done) { //checked ok
            //System.out.println("bids_done: "+i);
        }

        ArrayList<Integer> bids_todo = new ArrayList<Integer>();
        for (Integer i : bids_total) {

            if (bids_done.contains(i)) {
                //do nothing
            } else {
                //System.out.println("todo = " + i); //checked ok
                bids_todo.add(i);
            }
        }
        
        //make list_to_nest equal to bids_todo
        List<NestPath> templist = new ArrayList<NestPath>();
        for (Integer i : bids_todo) {
            for (NestPath j : list_to_nest) {
                if (j.bid == i) {
                    templist.add(j);
                    //System.out.println("todo list = " +j.bid); //checked ok
                }
            }
        }
        list_to_nest.clear();
        list_to_nest=templist;
        //System.out.println("we now have a list_to_nest from left over parts"); //checked ok
       
      
        
        

      
        writer0.write("Grotius Nest Output\n");
        for (String s : list_out) {
            writer0.write(s + "\n");
        }
       
        
        
        
        
        
            platenumber++;
         } while (list_to_nest.size()!=0); //end of loop

         writer0.close();
        
        
        
        
        
        System.out.println("\nstreaming completed..\n");
        File f1 = new File("ready.txt");
        if (!f1.exists()) {
            try {
                f1.createNewFile();
            } catch (IOException ex) {
                Logger.getLogger(grotius_nest_interface.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        Writer writer1 = null;
        try {
            writer1 = new FileWriter(f1, false);
        } catch (IOException ex) {
            Logger.getLogger(grotius_nest_interface.class.getName()).log(Level.SEVERE, null, ex);
        }
        writer1.write("Nesting completed !\n");
        writer1.close();

    }
    
    public static void nestcore(){
        
        
        
        
        
        
        
        
    }
}
