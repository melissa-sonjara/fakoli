<?php
/**
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 5/18/10
 */

Fakoli::using("survey");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	ajaxReturn("unknown survey");

$survey = new Survey($survey_id);
if($survey->isAuthor())
	$survey->setStatus("open");

ajaxReturn("OK");