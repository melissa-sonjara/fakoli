<?php
/*
 * survey_set_status.inc
 * 
 * Description: Nondisplay script to update survey status.
 *  
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 5/18/10
 */

Fakoli::using("survey");

$survey_id = checkNumeric($_GET["survey_id"]);
$status = checkNumeric($_GET["status"]);
$return = checkNumeric($_GET["return"]);

$redirect = (!$return) ? "/survey_list" : "/survey_manage?survey_id={$survey_id}";

if($survey_id AND $status)
{
	$survey = new Survey($survey_id);
	if($survey->isAuthor())
		$survey->setStatus($status);
}

redirect($redirect);