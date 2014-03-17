<?php

Fakoli::using("binding");
Fakoli::usingFeature("data_view");

$menu_item = "Bindings";

$bindings = Query::create(BindingTarget, "ORDER BY target_class")->execute();

$table = new DataListView($bindings, "bindings");

$table->column("Binding Target", "<a href='binding_form?binding_target_id={binding_target_id}'>{target_class}</a>", true)
->column("Associated Classes", "{associated_classes:/,/, /}", true);
 
$table->pageSize = 20;
$table->sortable = true;
$table->filter = true;
$table->emptyMessage = "No bindings have been created yet.";

$script .= $table->writeScript();

$table->drawView();
?>
<br/>
<a class='button' href='binding_form'>Add a New Binding</a>
