//TODO: 
//-nut und feder mehr spiel (zwischen lower und upper), nicht so wichtig
//button: auf einer seite abflachen oder mehr spiel (letzteres).verdickung duenner in Z, innenring mehr durchmesser


$fn=32;

module pcb()
{
	color("green")
		translate([-0.2,-0.25,-0.2])
		cube([47.9,67.5,2]);
	translate([1,1,1.5])					//components
		cube([45.5,65,3.1]);

	color("black")						//connecetor
		translate([36.75-6.25,44.3-2.3,1.6])
		{
			cube([12.5,4.6,4.4]);
			translate([0,0,4.3])
				cube([12.5,4.6,5]);
		}

	
	translate([41.75,35.9,1.6])		//reset button
	{
//		color("black")
//			cylinder(r=2, h=3.3);
		cylinder(r=0.75, h=10);
//		translate([-1.5,0,2.9])
//			#cube([3,7,0.3]);
	}
		

	color("black")
		translate([5.8,56,1.6])
			cylinder(r=1.5, h=3.1);
	translate([5.8,56,1.6+3])				//user button
	{
		//#cylinder(r=1.9, h=10);
		#cylinder(r=2.8, h=5);
		#cylinder(r=4.2, h=2);
		translate([0,0,3.5])
			#cylinder(r1=2.8, r2=4, h=2);
//		translate([-1.5,-24,0])
//			#cube([3,24,0.2]);

	}

	translate([5.55,62.7,1.6])			//led
	{
		cylinder(r=2.1, h=7);
		//#cylinder(r=3, h=1.5+3);
		translate([0,0,1.5+3+0.5])
			cylinder(r1=2.1, r2=3.9, h=3.5);
	}

	color("silver")						//accu
	{
		translate([11.5,-.2,-10.5])
			cube([35,67.2,10.5]);
			
		translate([17,-0.4,-8])
			cube([7,4,11]);
		translate([34,-0.4,-8])
			cube([7,4,11]);
	}

	color("violet")						//usb connector
	{
		translate([29-4.75,-3.5, -3.75])
			cube([9.5,10,4.5]);
		translate([29-4.5,-4, -3-0.5])
				cube([9,2.5,4]);
	}
	translate([29-4-2,-1.5-5, -3-3+0.75])
	{
		difference()
		{
			cube([8+4,5,3+6-1.5]);
			//translate([1,0,2-0.75])
			//	#cube([10,6,5]);
		}
		
	}
	
	translate([6,2,-6.3])				//antenne
		rotate(-90,[1,0,0])
			cylinder(r=5.7, h=10.4+0.75);
	color("black")
		translate([6,12.4+0.75,-6.3])
			rotate(-90,[1,0,0])
			{
				cylinder(r=5.7, h=54.25);
				translate([0,0,54.25])
				{
					cylinder(r1=5.7, r2=4.2, h=12);
					cylinder(r=2.5, h=30);
				}
			}
	translate([2,2,-9])
		cube([20,64.99,9]);


	translate([9.9,-10,-6.5])		//mount
		rotate(a=-90, v=[1,0,0])
			#cylinder(r=1.5,h=20);
	translate([1.85,-10,-6.5])		//mount
		rotate(a=-90, v=[1,0,0])
			#cylinder(r=1.5,h=20);

}

module button()
{
		cylinder(r=2.45, h=4);
		cylinder(r=3.8, h=1.5);
}

module case()
{
	difference()
	{
		union()
		{
			minkowski()
			{
				rotate(a=360/16, v=[0,0,1])
					sphere(r = 5.1, $fn=8);
				cube([43,63,11]);
			}
			translate([3.5,54,15.8])
				cylinder(r1=4.25, r2=3.4, h=1.2);
		}
		//translate([-6,-6,-6])
		//	cube([60,60,32]);
		//translate([30,-6,-6])
		//	cube([60,80,32]);
		translate([-2.25,-2,8.25])
			pcb();
	}

	translate([29-4-2-2.25+.5,-4.4, -3-3+0.75+8.25+.5])
	{
			difference()
			{
				cube([8+3,0.7,8-1.5]);
				translate([0.75,0,0.75])
					cube([8+3-1.5,1.25,8-1.5-1.5]);
				
			}
		
	}
}

module upperCase()
{
	difference()
	{
		case();
		translate([-6,-6,-6])
			cube([60,35,32]);
		translate([28.25,28,12])
			cube([12.5,12,5]);
		//translate([28.25-1,28,12])
			//cube([14.5,12,2]);
		translate([-2.25,28,4])
			cube([47.5,2.5,10]);
		translate([6,28,-3.25])
			cube([39.25,2.5,10]);
		translate([-2.25,-2,8.25])
			translate([6,12.4,-6.3])
				rotate(-90,[1,0,0])
					cylinder(r=6.1, h=20.1);

	}
}

module lowerCase()
{
	difference()	
	{
		case();
		upperCase();
	}

}

//pcb();
//case();

upperCase();
//lowerCase();
//button();



