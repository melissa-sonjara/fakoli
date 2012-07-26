<?php
Fakoli::using("svg_charts");

$chart = new Histogram("histogram", 600, 400, 50, 50, 400, 300, "standard");
$chart->setLabels(array("First", "Second", "Third", "Fourth"));
$chart->ticks = 5;
$chart->max = 100;

$series = new HistogramSeries("block", "Series 1", array(10, 40, 20, 80));
$chart->addSeries($series);

$series2 = new HistogramSeries("block", "Series 2", array(30, 20, 40, 90));
$chart->addSeries($series2);


$series3 = new HistogramSeries("block", "Series 3", array(20, 50, 40, 30));
$chart->addSeries($series3);


$series4 = new HistogramSeries("block", "Series 4", array(90, 20, 10, 50));
$chart->addSeries($series4);
//$pie->setRadius(150);
//$pie->setCenter(180,180);
//$pie->setLabelSize(16);
//$pie->setStrokeWidth(2);
//$pie->showLegend(true, 350, 20);
$chart->draw();
?>