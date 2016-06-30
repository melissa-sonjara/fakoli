<?php
Fakoli::using("svg_charts");

$chart = new RadarChart("radar", 600, 600, "standard");
$chart->setLabels(array("First", "Second", "Third", "Fourth", "Fifth"));

$chart->setLabelSize(16);
$chart->setStrokeWidth(2);
$chart->showLegend(true, 350, 20);

$series = new HistogramSeries("straight", "Series 1", array(10, 40, 20, 80, 100));
$series->set("areaFill", true);
$series->set("areaFillOpacity", 0.3);

$chart->addSeries($series);

echo "<h2>Radar Chart</h2>";

$chart->draw();