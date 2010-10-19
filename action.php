<?
require_once "include/config.inc";
require_once "cms/core.inc";

Fakoli::using("component");

$mgr = new ComponentManager();
$mgr->dispatchAction();

?>