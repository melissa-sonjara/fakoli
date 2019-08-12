<?php
class Glossary extends DataItem
{
    var $table ="glossary";
    var $primary_key = "glossary_id";
    
    var $fields = [
        "glossary_id"=>Number,
        "term"       =>String,
        "definition" =>HTML,
        "display"    =>Boolean
    ];
    
}
?>