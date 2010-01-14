<?php
require_once "../../include/config.inc";
require_once "../../include/permissions.inc";
require_once "../datamodel/publication.inc";
require_once "../datamodel/journal.inc";
require_once "../../framework/auto_form.inc";
require_once "include/publication_tabs.inc";

$menu_item = "Publications";

$publication_id = checkNumeric($_GET["publication_id"]);

$publication = new Publication();

if ($publication_id)
{
	$publication->load($publication_id);
}

$tabs = publicationTabs($publication_id);

$form = new AutoForm($publication);
$form->required("title", "publication_year", "abstract", "short_abstract", "journal_id");
$form->allowDelete=true;
$form->alias("pubmed_link","Link to Journal");

if ($publication->pdf_file)
{
	$form->annotate("pdf_file", "<a href='/download.php?publication_id={$publication_id}'>{$publication->pdf_file}</a>");
}

$journalSelect = new RelatedItemSelectFieldRenderer($form, "journal_id", "Journal", Journal, "ORDER BY title", "title", "journal_id", false);

$fileupload = new FileUploadFieldRenderer($form, "pdf_file", "Upload File", uploadPDF);  

if ($method == "POST")
{
	if ($form->save())
	{
		$tabs->queryString = "publication_id={$form->data->publication_id}";
		$tabs->next();
	} 
}
	
if ($publication_id)
{
	$publication->load($publication_id);
	$title = "Edit Publication: {$publication->title}";
}
else
{
	$title =  "Add New Publication";
}

$script = $form->writeScript();

require_once admin_begin_page;

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
$form->drawForm();
?>
</div><br/>
<?
require_once admin_end_page;


function uploadPDF($field, $publication)
{
	global $config;
	
	trace("uploadPDF() called for $field", 3);
	
	if (!$_FILES[$field]) 
	{
		trace("No upload record for $field", 2);
		return false;
	}
	if ($_FILES[$field]["name"]=="") 
	{
		trace("Upload name is empty", 2);
		return false;
	}

  	/* Copy across the uploaded file */

	trace("Upload Base: {$config['uploadbase']}", 3);
	trace("Upload Directory: {$config['uploaddir']}", 3);
	
	$dir = $config["uploaddir"];
	$name = $_FILES[$field]["name"];
	$file = "$dir/$name";
	
	trace ("Uploading file to {$config['uploadbase']}/$file", 3);
	
	if (!file_exists("{$config['uploadbase']}/$dir"))
	{
		// If the directory does not exist, create it 
		mkdir("{$config['uploadbase']}/$dir");
	}
	else if (file_exists("{$config['uploadbase']}/$file"))
	{
		// If a previous copy of the file already exists, remove it
		unlink("{$config['uploadbase']}/$file");
	}

  	move_uploaded_file($_FILES[$field]["tmp_name"], "{$config['uploadbase']}/$file");
  	chmod("{$config['uploadbase']}/$file", 0755);
 
  	$publication->pdf_file = "$file";
  	
  	return true;
}
?>