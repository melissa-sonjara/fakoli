<?php
/**\addtogroup section
 * @{
 */


Fakoli::using("site", "section");

class SectionContentPropertiesFieldRenderer extends FieldRenderer
{
	private $identifierField = null;
	
	function __construct($form, $field = "section_mapping", $label = "Section Mapping", $identifierField = "identifier")
	{
		$this->FieldRenderer($form);
	
		$this->identifierField = $identifierField;
		
		// JDG 7/31/2011 - allow override
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			$form->overrides[$field]['label'] = $label;
		}
		
		Fakoli::includeScript("/components/section/js/section_content_properties_manager.js");
	}
	
	function renderScript($field)
	{
		$obj = $this->parent->data;
		$id = $this->parent->id."_".$field;
?>
<script>
var sectionContentPropertyManager;

window.addEvent('domready', function()
{
	sectionContentPropertyManager = new SectionContentPropertiesManager('<?php echo $id?>');
});
</script>
<?
	}
	
	function renderField($field)
	{
		$obj = $this->parent->data;
		
		$this->_startField($field);
		
		$sites = Site::getSites();
		$sections = GroupedQuery::create(Section, "WHERE section_type='Content' ORDER BY section", "site_id")
								->execute();
		
		$id = $this->parent->id."_".$field;
		
		$identifier = $obj->get($this->identifierField);
		if ($identifier)
		{
			$sectionContent = IndexedQuery::create(SectionContent, "WHERE identifier=:i", "section_id")
								   ->bind(":i", $identifier)
								   ->execute();
		}
		else
		{
			$sectionContent = array();
		}
		
		$sectionContent = toJSONIndexed($sectionContent);
	?>
	<input type='hidden' name='<?echo $id?>' id='<?echo $id?>' value='<? echo htmlSafe($sectionContent)?>'/>
	<table id='<?echo $id?>_table' class='list'>
	<tbody>
	<?
		foreach($sites as $site)
		{
			$sectionsForSite = $sections[$site->site_id];
			if (!$sectionsForSite) continue;
?>
	 <tr>
	  <th class='subheading' colspan='3'><?echo $site->site_name?></th>
	 </tr>
<?	
			foreach($sectionsForSite as $s)
			{
?>
	<tr data-section_id='<?echo $s->section_id?>'>
	 <td><label><input type='checkbox'><? echo $s->section?></label></td>
	 <td><a class='button' href='#'><i class='fa-fw fas fa-ellipsis-h'></i></a></td>
	</tr>
<?
			}
		}
	?>
	</tbody>
	</table>
	<?
		$this->_endField($field);
	}
	
	function postProcess($field = "")
	{
		
		$id = $this->parent->id."_".$field;
		$json = $_POST[$id];
		
		trace("JSON: $json", 3);
		$items = fromJSON('SectionContent', $json);
		
		$tx = new DataTransaction();
		try 
		{
			foreach($items as $item)
			{
				if ($item->section_id && $item->identifier) 
				{
					$item->joinTransaction($tx);
					$item->save();
				}
			}
			
			$tx->commit();
		} 
		catch (Exception $e) 
		{
			$tx->rollback();
			throw $e;
		}
	}
}

/**@}*/?>