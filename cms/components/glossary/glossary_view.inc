<?php
class GlossaryView{
    
    var $glossary;
    var $display = null;
    
    function __construct($glossary)
    {
        $this->glossary = $glossary;
        $this->display = null;
    }
    
    function draw()
    {
        $display = "<dl>";
        foreach ($this->glossary as $term)
        {
            $display .="<dt>{$term->term}</dt>";
            $display .="<dd>{$term->definition}</dd>";
        }
        $display .='<dl>';
        echo $display;
    }
    
    function glossaryJsonFormat()
    {
       $glossary = $this->glossary;
       echo toJSON($terms);
    }
}