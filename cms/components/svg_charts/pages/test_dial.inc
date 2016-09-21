<?php
Fakoli::using("svg_charts");

$dial = new Dial("dial_test", 400, 450, 66, "66%");
$dial->setRadius(150);
$dial->setCenter(200,180);
$dial->setLabelSize(48);
$dial->setLabelPosition(200, 400);
$dial->setStrokeWidth(6);
$dial->setColorRange('#134b8f');
$dial->shadow = true;
$dial->scaleToFit = true;

$dial2 = new Dial("dial_test2", 400, 450, 33, "33%");
$dial2->setRadius(150);
$dial2->setCenter(200,180);
$dial2->setLabelSize(48);
$dial2->setLabelPosition(200, 400);
$dial2->setStrokeWidth(6);
$dial2->setColorRange('#134b8f');
$dial2->shadow = true;
$dial2->scaleToFit = true;


$dial3 = new Dial("dial_test3", 400, 450, 50, "50%");
$dial3->setRadius(150);
$dial3->setCenter(200,180);
$dial3->setLabelSize(48);
$dial3->setLabelPosition(200, 400);
$dial3->setStrokeWidth(6);
$dial3->setColorRange('#134b8f');
$dial3->shadow = true;
$dial3->scaleToFit = true;


$dial4 = new Dial("dial_test4", 400, 450, 0, "0%");
$dial4->setRadius(150);
$dial4->setCenter(200,180);
$dial4->setLabelSize(48);
$dial4->setLabelPosition(200, 400);
$dial4->setStrokeWidth(6);
$dial4->setColorRange('#134b8f');
$dial4->shadow = true;
$dial4->scaleToFit = true;

?>
<div style='float:left;width:25%'><?$dial->draw();?></div><div style='float:left;width:25%'><?$dial2->draw();?></div><div style='float:left;width:25%'><?$dial3->draw();?></div><div style='float:left;width:25%'><?$dial4->draw();?></div>