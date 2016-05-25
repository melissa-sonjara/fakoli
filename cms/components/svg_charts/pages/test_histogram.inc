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
$chart->draw();


$chart2 = new Histogram("histogram2", 600, 400, 50, 50, 400, 300, "standard");
$chart2->setLabels(array("First", "Second", "Third", "Fourth"));
$chart2->ticks = 5;
$chart2->max = 100;
$chart2->orientation = "horizontal";

$series = new HistogramSeries("block", "Series 1", array(10, 40, 20, 80));
$chart2->addSeries($series);

$series2 = new HistogramSeries("block", "Series 2", array(30, 20, 40, 90));
$chart2->addSeries($series2);


$series3 = new HistogramSeries("block", "Series 3", array(20, 50, 40, 30));
$chart2->addSeries($series3);


$series4 = new HistogramSeries("block", "Series 4", array(90, 20, 10, 50));
$chart2->addSeries($series4);
$chart2->draw();
?>