<?php
Fakoli::using("sharing");
$share_id = checkNumeric($_GET["share_id"]);
$work_area_id = checkNumeric($_GET["work_area_id"]);

if (!$work_area_id || !$share_id)
{
	Fakoli::end("Missing Parameter");
}

$workArea = new WorkArea($work_area_id);
$share = new ShareToken($share_id);

if (!checkRole("admin,data") && $user->user_id != $share->user_id && !$workArea->isManager())
{
	Fakoli::end("Permission Denied");
}

$share->active = !$share->active;
$share->last_updated = now();
$share->last_updated_by_id = $user->user_id;
$share->save();

Fakoli::end("OK");