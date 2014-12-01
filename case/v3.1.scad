$fn=32;

module pcb()
{
	color("green")
		cube([47.5,67,1.6]);
	translate([1,1,1.5])					//components
		cube([45.5,65,3.1]);

	color("black")						//connecetor
		translate([36.75-6.25,44.3-2.3,1.6])
		{
			cube([12.5,4.6,4.4]);
			translate([0,0,4.3])
				#cube([12.5,4.6,5]);
		}

	
	translate([41.75,35.9,1.6])		//reset button
	{
		color("black")
			cylinder(r=2, h=3.3);
		#cylinder(r=0.75, h=10);
		translate([-1.5,0,2.9])
			#cube([3,7,0.3]);
	}
		

	color("black")
		translate([5.8,56,1.6])
			cylinder(r=1.5, h=3.1);
	translate([5.8,56,1.6+3])				//user button
	{
		//#cylinder(r=1.9, h=10);
		#cylinder(r=2.8, h=5);
		#cylinder(r=3.5, h=2);
		translate([0,0,3.5])
			#cylinder(r1=2.8, r2=4, h=2);
		translate([-1.5,-24,0])
			#cube([3,24,0.2]);

	}

	translate([5.55,62.7,1.6])			//led
	{
		#cylinder(r=2.1, h=7);
		#cylinder(r=3, h=1.5+3);
		translate([0,0,1.5+3+0.5])
			#cylinder(r1=2.1, r2=3.9, h=3.5);
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
		translate([29-4,-1.5, -3])
			cube([8,5,3]);
	translate([29-4-3,-1.5-5, -3-3])
		#cube([8+6,5,3+6]);

	
	translate([5.7,2,-6.3])				//antenne
		rotate(-90,[1,0,0])
			#cylinder(r=5.5, h=10.4);
	color("black")
		translate([5.7,12.4,-6.3])
			rotate(-90,[1,0,0])
			{
				cylinder(r=5.5, h=54);
				translate([0,0,54])
				{
					cylinder(r1=5.5, r2=4, h=12);
					cylinder(r=2.5, h=30);
				}
			}
	translate([2,2,-9])
		#cube([20,64.99,9]);

}

module button()
{
		cylinder(r=2.8, h=4);
		cylinder(r=3.5, h=1.8);
}

module case()
{
	difference()
	{
		union()
		{
			minkowski()
			{
				sphere(r = 5, $fn=16);
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
		translate([28.25-1,28,12])
			cube([14.5,12,2]);
		translate([-2.25,28,4])
			#cube([47.5,2.5,10]);
		translate([6,28,-3.25])
			cube([39.25,2.5,10]);
		translate([-2.25,-2,8.25])
			translate([5.7,12.4,-6.3])
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
case();

//upperCase();
//lowerCase();
//button();



