<?php
Fakoli::using("rating");

$url = $_GET["url"];
$rating = checkNumeric($_GET["rating"]);

if (!$user) return;

$pageRating = PageRating::fromURL($url);

$pageRating->rating = $rating;
$pageRating->save();

$stats = PageRatingStatistics::fromURL($url);

Fakoli::end($stats->toJSON());
