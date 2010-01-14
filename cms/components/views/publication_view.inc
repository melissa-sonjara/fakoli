<?php
require_once realpath(dirname(__FILE__)."/../datamodel/publication.inc");
require_once realpath(dirname(__FILE__)."/../datamodel/author.inc");
require_once realpath(dirname(__FILE__)."/../datamodel/focus.inc");
require_once realpath(dirname(__FILE__)."/../datamodel/region.inc");
require_once realpath(dirname(__FILE__)."/../datamodel/sample.inc");
require_once realpath(dirname(__FILE__)."/../datamodel/setting.inc");
require_once realpath(dirname(__FILE__)."/../datamodel/journal.inc");
require_once realpath(dirname(__FILE__)."/../datamodel/organization.inc");

class PublicationView
{
	var $publication;
	
	function PublicationView($publication)
	{
		$this->publication = $publication;
	}
	
	function drawView()
	{
		$authors = $this->publication->Authors("ORDER BY sort_order");
		$foci = $this->publication->Foci("ORDER BY focus");
		$regions = $this->publication->Regions("ORDER BY region_id");
		$samples = $this->publication->Samples("ORDER BY sample");
		$settings = $this->publication->Settings("ORDER BY setting");
		$journal = $this->publication->Journal();
		
?>
		<h3 style="border: none"><?echo $this->publication->title ?></h3>
		
		
		<h4>Authors</h4>
		<table class="layout" width="100%">
<?
		foreach($authors as $author)
		{
			$org = $author->Organization();
?>
		 <tr>
		  <td style="vertical-align: top" nowrap="nowrap"><b><?echo $author->author_name ?></b></td>
		  <td style="vertical-align: top"><?echo $org->name ?> <?echo $org->address ?></td>
		 </tr>
<?
		}
?>

		</table>

<?
		if ($journal)
		{
			if ($journal->url)
			{
				$journalText = "<a href='{$journal->url}' target='_blank'>{$journal->title}</a>";
			}
			else
			{
				$journalText = $journal->title;
			}
?>
		<p><label>Journal: </label><?echo $journalText ?></p>
<?
		}
?>
		<p><label>Publication Year:</label> <?echo $this->publication->publication_year ?></p>
<?
	//global $user;
	//if ($user)
	//{
?>
	
<?
	//}
	//else
	//{
?>
	<p><label>Weblink:</label> <a href="<? echo $this->publication->pubmed_link ?>" target="_blank"><? echo $this->publication->pubmed_link ?></a></p>
<? 
	//}
	
	global $user;
	global $config;
	
	if ($config['allow_pdf_download'] && $user)
	{
?>
	<p><label>Members Only: <a href="/download.php?publication_id=<?echo $this->publication->publication_id?>">Download full article</a>&nbsp;<img src="/framework/images/pdf_icon.png" alt="PDF Format" style="display: inline-block; vertical-align: middle"/></p>
<? 
	}
?>
		<h4>Abstract</h4>
		
		<p>
		<?echo $this->publication->abstract ?>
</p>
<?
		if (trim($this->publication->intervention))
		{
?>
	<h4>Intervention</h4>
	<p>
		<?echo $this->publication->intervention ?></p>
		
<? 
		}
		
		if (trim($this->publication->outcomes))
		{
		?>		
		<h4>Outcomes</h4>
		<p><?echo $this->publication->outcomes ?></p>
<?		}
?>
			
<? 
		if (trim($this->publication->effect))
		{
		?>	
	<h4>Effect</h4>
	<p>	<?echo $this->publication->effect ?></p>
<?		}
?>

<h3>Study Details</h3>
<p><label>Keywords:</label> <?echo $this->publication->keywords ?></p>

<?
		if (count($regions) > 0)
		{
?><p><label>Regions:</label> <?echo formatItems($regions, "{region}", ", ") ?></p>
<?
		}
?>
<?
		if (count($foci) > 0)
		{
?><p><label>Focus:</label> <?echo formatItems($foci, "{focus}", ", ") ?></p>
<?
		}
		
		
		if (count($samples) > 0)
		{
?><p><label>Study Sample:</label> <?echo formatItems($samples, "{sample}", ", ") ?></p>
<?
		}
			
		if (count($settings) > 0)
		{
?><p><label>Study Setting:</label> <?echo formatItems($settings, "{setting}", ", ") ?></p>

<h4>Disclaimer</h4>
<p><i><?echo $this->publication->title ?></i> is legal property of <?echo $journalText = $journal->title?> and is for use solely by designees of the National Council of Urban Indian Health as established through agreement with the publisher/owner of this document.  


<?


		}
?><br/><br/><?


	}
	
	
}



?>