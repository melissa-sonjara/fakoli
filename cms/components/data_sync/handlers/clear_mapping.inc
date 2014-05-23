<?php

$class = $_SESSION["data_import_class"];
$import_profile_id = checkNumeric($_GET["import_profile_id"]);

if (!$import_profile_id) $import_profile_id = 0;

if ($class)
{
	$doomed = new DataImportFieldMapping();
	$doomed->delete("WHERE class='$class' AND import_profile_id={$import_profile_id}");
	if ($import_profile_id)
	{
		$profile = new DataImportProfile($import_profile_id);
		$profile->delete();
	}
	unset($_SESSION["import_profile_id"]);
}

$redirect = $_SERVER["HTTP_REFERER"];
if (!$redirect) $redirect = "/admin/data_import_field_mapping";
redirect($redirect);
