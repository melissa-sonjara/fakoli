<?php
Fakoli::using("sharing");

$guid = Fakoli::get("share_token");
if (!$guid) throw new FakoliException("Missing token");

SharingManager::drawShare($guid);
