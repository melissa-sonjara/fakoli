<?php

Fakoli::using("taxonomy");
Fakoli::usingFeature("auto_form");

$menu_item = "Bindings";

$binding_target_id = checkNumeric($_GET["binding_target_id"]);

$bindingTarget = new BindingTarget();

if ($binding_target_id)
{
	$bindingTarget->load($binding_target_id);
}

$form = new AutoForm($bindingTarget);
$form->required("target_class", "associated_classes");
$form->unique("target_class", "The binding for this class has already been defined");

$form->allowDelete = true;

$classes =  TaxonomyManager::getRegisteredClassOptions();

new CheckListFieldRenderer($form, "associated_classes", "Associated Content Items", $classes);

$targets = TaxonomyManager::getBindableClassOptions();

new SelectFieldRenderer($form, "target_class", "Target Class", $targets);

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("/admin/bindings");
	}
}

$script .= $form->writeScript();

echo $form->drawForm();
