// Set to MM

// Size 200mm x 100mm x 50mm
// w x d x h

buttonSize = 24;
rowstart = 100;
sidespace = 110;

difference(){
  cube([210, 125, 5]);
    // Left Hand Side
  translate([20,rowstart, 0]) cylinder(r=buttonSize/2,h=100);
  translate([50,rowstart, 0]) cylinder(r=buttonSize/2,h=100);
  translate([76,rowstart - 15, 0]) cylinder(r=buttonSize/2,h=100);
    // Right Hand Side
  translate([24 + sidespace,rowstart - 15, 0]) cylinder(r=buttonSize/2,h=100);
  translate([50 + sidespace,rowstart, 0]) cylinder(r=buttonSize/2,h=100);
  translate([80 + sidespace,rowstart, 0]) cylinder(r=buttonSize/2,h=100);
    // Middle Area
  translate([85 - buttonSize / 2, rowstart - 69 - buttonSize / 2, 0]) cube(buttonSize,h=100);
  translate([15 + sidespace - buttonSize / 2,rowstart - 69 - buttonSize / 2, 0]) cube(buttonSize,h=100);
}
