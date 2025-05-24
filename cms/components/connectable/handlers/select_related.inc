<?php
Fakoli::using("connectable");
Fakoli::usingFeature("auto_form");

list($sourceClass, $sourceID) = explode(":", $_GET["from"]);
checkIdentifier($sourceClass);
checkNumeric($sourceID);

$to = checkIdentifier($_GET["to"]);

$source = new $sourceClass;
if (!$source instanceof Connectable)
{
	throw new FakoliException("$source is not a connectable class");
}

$source->filter = new InclusionFilter($source->getPrimaryKey());
$source->load($sourceID);

$form = new AutoForm($source, "POST", "/action/connectable/select_related?from={$_GET['from']}&to=$to", "ConnectableSelect");

$form->ajaxSubmit("function(result) { ConnectableNavigator.selectResult(result);}", "function() {document.id('{$form->id}_error').set('text','Failed to communicate with server'); }");

$form->submitLabel = "Save";

$target = new $to;

$from = get_class($source).":".$source->get($source->getPrimaryKey());
$createURL = ConnectableManager::$contextRouter . "?from=$from&to=$to";
$isScript = false;;

if (method_exists($target, 'getConnectableCreateHandler'))
{
	$createHandler = $target->getConnectableCreateHandler();
	$createURL = "{$createHandler}('{$createURL}'); return false;";
	$isScript = true;
}

$form->button("Create New ".$target->prettifyClassName(), $createURL, null, $isScript);

$selectFieldRenderer = new ConnectableTableFieldRenderer($form, "related_items", "Related Items", $to);

if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::end("OK");
	}
	else
	{
		Fakoli::end($form->msg);
	}
}

$script .= $form->writeScript();
$form->drawForm();
