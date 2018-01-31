/* Hexapod by A.J. van de Werken */
/* May 2016                      */

servo_dist = 27.6;
servo_width = 11.5+0.5;
servo_length = 23+0.5;
servo_height = 15+0.5;

driverboard_width = 19.5;
driverboard_length = 56;
nodemcuboard_width = 25;
nodemcuboard_length  = 52;
dcdcboard_length = 16;
dcdcboard_width = 34;

tibia_thickness = 4;
femur_thickness = 4;

$fn=36;

module dcdcconverter(holes=false)
{
   if( !holes )
   {
        union()
        {
            translate([-dcdcboard_length/2,dcdcboard_width/2,0]) cylinder( d=5, h=6 );
            translate([-dcdcboard_length/2,-dcdcboard_width/2,0]) cylinder( d=5, h=6 );
            translate([dcdcboard_length/2,-dcdcboard_width/2,0]) cylinder( d=5, h=6 );
            translate([dcdcboard_length/2,dcdcboard_width/2,0]) cylinder( d=5, h=6 );
        }
    }
    else
    {
        union()
        {
            translate([-dcdcboard_length/2,dcdcboard_width/2,0]) cylinder( d=2, h=6 );
            translate([-dcdcboard_length/2,-dcdcboard_width/2,0]) cylinder( d=2, h=6 );
            translate([dcdcboard_length/2,-dcdcboard_width/2,0]) cylinder( d=2, h=6 );
            translate([dcdcboard_length/2,dcdcboard_width/2,0]) cylinder( d=2, h=6 );
        }
    }
}

module nodemcu(holes=false)
{
    if( !holes )
    {
        union()
        {
            translate([-nodemcuboard_length/2,nodemcuboard_width/2,0]) cylinder( d=5, h=6 );
            translate([-nodemcuboard_length/2,-nodemcuboard_width/2,0]) cylinder( d=5, h=6 );
            translate([nodemcuboard_length/2,-nodemcuboard_width/2,0]) cylinder( d=5, h=6 );
            translate([nodemcuboard_length/2,nodemcuboard_width/2,0]) cylinder( d=5, h=6 );
        }
    }
    else
    {
        union()
        {
            translate([-nodemcuboard_length/2,nodemcuboard_width/2,0]) cylinder( d=2, h=6 );
            translate([-nodemcuboard_length/2,-nodemcuboard_width/2,0]) cylinder( d=2, h=6 );
            translate([nodemcuboard_length/2,-nodemcuboard_width/2,0]) cylinder( d=2, h=6 );
            translate([nodemcuboard_length/2,nodemcuboard_width/2,0]) cylinder( d=2, h=6 );

        }
    }
}

module driver(holes=false)
{
    if(!holes)
    {
        union()
        {
            translate([-driverboard_length/2,driverboard_width/2,0]) cylinder( d=5, h=6 );
            translate([-driverboard_length/2,-driverboard_width/2,0]) cylinder( d=5, h=6 );
            translate([driverboard_length/2,-driverboard_width/2,0]) cylinder( d=5, h=6 );
            translate([driverboard_length/2,driverboard_width/2,0]) cylinder( d=5, h=6 );
        }
    }
    else
    {
        union()
        {
            translate([-driverboard_length/2,driverboard_width/2,0]) cylinder( d=2, h=6 );
            translate([-driverboard_length/2,-driverboard_width/2,0]) cylinder( d=2, h=6 );
            translate([driverboard_length/2,-driverboard_width/2,0]) cylinder( d=2, h=6 );
            translate([driverboard_length/2,driverboard_width/2,0]) cylinder( d=2, h=6 );
        }
    }
}

module servo()
{
    translate( [-servo_length/2, -servo_width/2, -servo_height/2 ] )
    cube( [servo_length, servo_width, servo_height ] );
    translate( [-servo_dist/2+0.3, 0, -1.5*servo_height ] )
    cylinder( d=2, h=servo_height*2, $fn=18 );
    translate( [servo_dist/2-0.3, 0, -1.5*servo_height ] )
    cylinder( d=2, h=servo_height*2, $fn=18 );
}


module tibia()
{
    difference()
    {
       hull()
        {
            cylinder( r=2, h=tibia_thickness);
            intersection()
            {
                translate( [-20,0,0]) cube([20,100,tibia_thickness] );
                translate( [190,40,0]) cylinder( r=195, h = tibia_thickness, $fn=180 );
            }
            translate( [5,75,0]) cylinder( r=10, h =tibia_thickness);
            translate( [20,85,0]) cylinder( r=10, h =tibia_thickness);
        }
        // Holes
        translate( [89,17,-1]) cylinder( r=88, h = tibia_thickness+2, $fn=180 );
        translate( [13,81,]) rotate([0,0,35]) servo();
    }
}



module femur()
{
    difference()
    {
        union()
        {
            translate( [-19,0,0] ) cylinder( d=22, h=femur_thickness, $fn=72 );
            translate( [ 19,0,0] ) cylinder( d=22, h=femur_thickness, $fn=72 );
            intersection()
            {
                difference()
                {
                    translate( [ 0,-10,0] ) cylinder( r=25, h=femur_thickness, $fn=60 );
                    translate( [ 0,-14,0] ) cylinder( r=19, h=femur_thickness, $fn=60 );
                }
                translate( [-60,0,0] ) cube( [120,60,femur_thickness]);
            }
        }

        for( t=[-19,19] )
        translate( [t,0,0] )
        { 
            translate([0,0,3]) cylinder( d=19.5, h=femur_thickness );
            cylinder( d=6, h=femur_thickness );
            for( a=[0:90:359] ) rotate([0,0,a]) translate([6.3,0,0]) cylinder( d=2.5, h=femur_thickness );
        }
        
    }
}

module coxa()
{
    rotate([90,0,0]) translate([-35,-2.75,0]) 
    intersection()
    {
        difference()
        {
            union()
            {
                translate([23,8.75,0]) cylinder( r=25,h=26, $fn=72 );
                translate([42,17.5,12]) rotate([90,0,0])cylinder( r=25,h=17.5, $fn=96 );
            }

            
            cube( [71,0,35] );
            translate( [6,0,6] ) cube( [23,17.5,35] );
            translate( [17.5,8.75,27.5-13]) servo();

            translate( [11.5,8.75,0] ) cylinder( d=3,h=10, $fn=18 );
            translate( [11.5,8.75,4] ) cylinder( d=6.2,h=10, $fn=18 );
       
            translate([70,25,22.5])
            rotate( [-90,0,0] )    
            rotate( [0,180,0] )
            {
                translate( [20.5,10.75,16]) servo();
                translate( [0,0,26] ) cube( [35,17.5,10] );
            }    
            
            translate( [35, 6.75, -1] ) cube([40,9,26]);
            
        }
        translate([-10,2.75,0]) cube( [90,12,22] );
    } 
}

module bottom()
{
    r1 =68;
    r2= 51;
    
    difference()
    {
        union()
        {
            difference()
            {   // Base
                rotate([0,0,30]) cylinder( r=r1, $fn=6, h=4 );
                translate([0,0,-1]) rotate([0,0,30]) cylinder( r=r2, $fn=6, h=6 );
            }
            
            for( a=[30:60:359]) 
            {   // Hips
                rotate([0,0,a]) translate( [r1, 0, 0] ) cylinder( d=20, h=4, $fn=72 );
                rotate([0,0,a]) translate( [r1, 0, 4] ) cylinder( d1=20 ,d2=14 , h=1, $fn=72 );
            }
            difference()
            {   // switch
                translate([41,0,2]) cube([18,34,4],  center=true );
                translate([43,0,0]) cube([9,14,10], center=true );
            }    
           
            // Boards support
            difference()
            {
                translate([-38,0,2]) cube([30,36,4], center=true );
                translate([-35,0,2]) cube([26,24,5], center=true );
            }
            translate([-20,0,2]) cube([6,100,4], center=true );
            

            translate([7,-28.5,0]) driver();
            translate([7,28.5,0]) driver();
            translate([7,0,0]) nodemcu();
            translate([-37,0,0]) dcdcconverter();
        }

        // Bearing holes
        for( a=[30:60:359]) 
        {
            rotate([0,0,a]) translate( [r1, 0, 1] ) cylinder( d=10.25, h=5, $fn=36 );
            rotate([0,0,a]) translate( [r1, 0, -1] ) cylinder( d=8, h=6, $fn=36 );
        }
        
        // Standoffs
        for( a=[0:60:359]) 
        {
            rotate([0,0,a]) translate( [r2-1.5, 0, 0] ) cylinder( d=3.2, h=5 );
        }

        // Holes
        translate([7,-28.5,0]) driver(true);
        translate([7,28.5,0]) driver(true);
        translate([7,0,0]) nodemcu(true);
        translate([-37,0,0]) dcdcconverter(true);

    }
   

        
}

module top()
{
    r1 =68;
    r2= 56;
    r3= 50;
    
    difference()
    {
        rotate([0,0,30]) union()
        {


            difference()
            {   // Base
                cylinder( r=r1, $fn=6, h=4 );
                translate([0,0,-1]) cylinder( r=r3, $fn=6, h=6 );
             }
            
            for( a=[0:60:359]) 
            {   // Hip joints
                rotate([0,0,a]) translate( [r1, 0, 0] ) cylinder( d=20, h=4, $fn=72 );
                rotate([0,0,a]) translate( [r1, 0, 0] ) cylinder( d=25, h=6, $fn=72 );
            }     
      
            cube([100,30,6]);
            
        }

        // Bearing holes
        for( a=[30:60:359]) 
        {
            rotate([0,0,a]) translate( [r1, 0, 5] ) cylinder( d=19.55, h=5, $fn=36 );
            rotate([0,0,a]) translate( [r1, 0, 0] ) cylinder( d=6, h=10, $fn=36 );
            rotate([0,0,a]) translate( [r1, 0, 0] )for( a=[0:90:359] ) rotate([0,0,a]) translate([6.3,0,0]) cylinder( d=2.5, h=10 );
        }
        // Standoffs
        for( a=[0:60:359]) 
        {
            rotate([0,0,a]) translate( [r2-1.5, 0, 0] ) cylinder( d=3.2, h=5 );
        }

    }
}



//scale( [ 1,1,1] )tibia(); // left
//scale( [ -1,1,1] )tibia(); // right
//femur();
//scale( [ 1,1,1] ) coxa(); // left
//scale( [-1,1,1] ) coxa(); // right
//projection(cut = true) translate([0,0,-4]) bottom();
projection(cut = true) translate([0,0,-1]) top();

