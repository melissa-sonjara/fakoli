<?php

Fakoli::using("process");

$id = $_GET["id"];

$mgr = new ProcessManager($id);

$progress = new ProgressData();

$progress->status = $mgr->get("status");
$progress->message = $mgr->get("message");
$progress->percentage = $mgr->get("percentage");

echo $progress->toJSON();
?>