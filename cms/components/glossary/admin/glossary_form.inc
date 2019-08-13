<?php
Fakoli::using("glossary");
Fakoli::usingFeature("auto_form");

$menu_item = "Glossary";

$glossary_id = checkNumeric($_GET["glossary_id"]);

$glossary = new Glossary();

if($glossary_id){
    $glossary->load($glossary_id);
}

$form = new AutoForm($glossary);
$form->required("term");
$form->allowDelete = true;

if($method=="POST")
{
    if($form->save())
    {
        redirect("glossaries");
    }
}

$script .= $form->writeScript();
$form->drawForm();
?>