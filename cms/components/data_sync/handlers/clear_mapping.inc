<?php

$class = $_SESSION["data_import_class"];

if ($class)
{
	$doomed = new DataImportFieldMapping();
	$doomed->delete("WHERE class='$class'");
}

redirect("data_import_field_mapping");
