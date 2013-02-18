<?php
?>
<h2>Templates</h2>
The site now uses full page templates with substitution points in the code. Pages are associated with templates, and the associated template is used to generate the page. 

The templates are basically the plain HTML for the page, and can be edited in Dreamweaver, Notepad++ or whatever. Substitution points are denoted with {}s. 
<ul>
<li>	{menu:identifier} substitutes the associated menu structure that is identified by unique <i>identifier</i></li>
<li>	{fieldname} substitutes field values from the Page object (i.e. page_title, description, etc.)</li>
<li>	{position:identifier} substitutes the associated Modules for the Page.</li>
</ul>