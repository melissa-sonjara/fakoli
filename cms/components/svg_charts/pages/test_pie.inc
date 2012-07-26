<?php
Fakoli::using("svg_charts");

$pie = new PieChart("pie_test", 600, 600, array(10,20,30,80), array("First", "Second", "Third", "Fourth"), "standard");
$pie->setRadius(150);
$pie->setCenter(180,180);
$pie->setLabelSize(16);
$pie->setStrokeWidth(2);
$pie->showLegend(true, 350, 20);
$pie->emboss = true;
$pie->shadow = false;
$pie->draw();
?>