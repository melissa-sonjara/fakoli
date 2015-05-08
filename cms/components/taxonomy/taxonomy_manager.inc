<?php
/**************************************************************

 Copyright (c) 2014 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

Fakoli::usingFeature("search_form");

class TaxonomySerializationHandler
{	
	function export()
	{
		$xml  = "\n<TaxonomyMap>";
		$xml .= SerializationManager::serialize(Taxonomy, "ORDER BY taxonomy_id");
		$xml .= SerializationManager::serialize(TaxonomyTerm, "ORDER BY term_id");
		$xml .= "</TaxonomyMap>";
		
		return $xml;
	}
	
	function import($doc, $tx)
	{
		SerializationManager::unserialize(Taxonomy, $doc, $tx);
		SerializationManager::unserialize(TaxonomyTerm, $doc, $tx);		
	}		
}

class TaxonomySearchParameterHandler extends SearchParameterHandler
{
	var $cl;
	
	function __construct($class)
	{
		$this->cl = $class;
	}
	
	function member($name, $set)
	{
		$values = $set;
		if(is_array($set))
			$values = implode(",", array_keys($set));

		trace("## Generating taxonomy terms clause for $name using values ($values)", 3);
		
		return "$name IN (SELECT id FROM taxonomy_term_association WHERE class='{$this->cl}' AND term_id IN ($set))";
	}
}

/**
 * TaxonomyManager provides the internal API for working with taxonomies,
 * facets and bindings. These methods are designed to bridge the gap between
 * the user-extensible taxonomy schemas and the defined items in your data model.
 * TaxonomyManager also provides event handlers and common functionality for the 
 * taxonomy component.
 */
class TaxonomyManager
{
	static $bindableClasses = null;
	
	/**
	 * Create a new TaxonomyManager instance. Generally not required as all methods are
	 * static.
	 */
	function __construct()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}

	static function getTabs($key)
	{
		$tabs = array(	"Taxonomy Definition"	=>	"/admin/taxonomy_form",
						"Terms"					=>	"/admin/taxonomy_terms");
		
		$qs = ($key) ? "taxonomy_id=$key" : ""; 
		return new TabBar("tabs", $tabs, $qs);
	}
	
	/**
	 * Retrieved the list of DataItem classes that have registered as supporting taxonomy term associations.
	 * @return array of class names
	 */
	static function getRegisteredClasses()
	{
		$registeredClasses = array();
		
		$registeredClasses = ComponentManager::fireEvent("RegisterTaxonomyClasses", $registeredClasses);
		
		return $registeredClasses;
	}
	
	/**
	 * Retrieves the list of DataItem classes that have registered as supporting taxonomy term associations
	 * in an associated array format suitable for use with a SelectFieldRenderer or CheckListFieldRenderer.
	 * @return array
	 */
	static function getRegisteredClassOptions()
	{
		$classes = TaxonomyManager::getRegisteredClasses();
		
		$options = array();
		
		foreach($classes as $class)
		{
			$proto = new $class;
			$options[$class] = $proto->prettifyClassName(true);
		}
		
		return $options;
	}
	
	static function getBindableClasses()
	{
		if (!TaxonomyManager::$bindableClasses)
		{
			$bindableClasses = array();
			ComponentManager::fireEvent("RegisterBindableClasses");
		}
		
		return TaxonomyManager::$bindableClasses;
	}
	
	static function registerBindableClass($class, $format, $constraint = "", $fieldRenderer = BindingTargetFieldRenderer, $facetFilter = BindingTargetFacetFilter)
	{
		TaxonomyManager::$bindableClasses[$class] = array("format" => $format, "constraint" => $constraint, 
														  "field_renderer" => $fieldRenderer, "facet_filter" => $facetFilter);
	}
	
	static function getBindableClassOptions()
	{
		$classes = TaxonomyManager::getBindableClasses();
	
		$options = array();
	
		foreach($classes as $class => $format)
		{
			$proto = new $class;
			$options[$class] = $proto->prettifyClassName();
		}
	
		return $options;
	}
		
	static function getBindingOptions($bindingTarget)
	{
		TaxonomyManager::getBindableClasses();
		
		$class = is_object($bindingTarget) ? get_class($bindingTarget) : $bindingTarget;
		$bindingDetails = TaxonomyManager::$bindableClasses[$class];
		if (!$bindingDetails) 
		{
			trace("## $class is not registered as a binding target", 1);
			return array();
		}
		
		$targets = Query::create($class, $bindingDetails["constraint"])->execute();
		
		return $targets;
	}
	
	static function getBindingOptionFormat($bindingTarget)
	{
		$classes = TaxonomyManager::getBindableClasses();
		return $classes[$bindingTarget->target_class]["format"];
	}
	
	static function getBindingFieldRendererClass($bindingTarget)
	{
		$classes = TaxonomyManager::getBindableClasses();
		return $classes[$bindingTarget->target_class]["field_renderer"];
	}
	
	static function getBindingFacetFilterClass($bindingTarget)
	{
		$classes = TaxonomyManager::getBindableClasses();
		return $classes[$bindingTarget->target_class]["facet_filter"];
	}
		
	static function addTaxonomyFieldRenderers($form, $showSelectAll = true, $group = null, $limitTo = null)
	{
		$renderers = TaxonomyManager::addBindingFieldRenderers($form, $showSelectAll, $group, $limitTo);
		
		$class = get_class($form->data);
		
		$taxonomies = Query::create(Taxonomy, "")->execute();
		
		foreach($taxonomies as $taxonomy)
		{
			$classes = explode(",", $taxonomy->associated_classes);
			if (array_search($class, $classes) !== FALSE)
			{
				if (is_array($limitTo) && !array_search($taxonomy->identifier, $limitTo))
				{
					continue;
				}
				
				trace("Adding TaxonomyTermFieldRenderer for {$taxonomy->identifier}", 3);
				$renderer = new TaxonomyTermFieldRenderer($form, $taxonomy);
				$renderer->setSize(400, 125);
				$renderer->resizable = true;
				$renderer->showSelectAll = $showSelectAll;
				
				if ($group)
				{
					$form->group($group, $taxonomy->identifier);
				}
				
				if ($form instanceof SearchForm)
				{
					$form->params->setHandler($taxonomy->identifier, new TaxonomySearchParameterHandler($class));
					$form->setMatchingMode("member", $taxonomy->identifier);
				}
				
				$renderers[] = $renderer;
			}
		}
		
		return $renderers;
	}
	
	static function addBindingFieldRenderers($form, $showSelectAll = true, $group = null)
	{
		TaxonomyManager::getBindableClasses();
		
		$class = get_class($form->data);
		
		$targets = Query::create(BindingTarget, "")->execute();
		
		trace("### Found ".count($targets)." binding targets", 3);
		$renderers = array();
		
		foreach($targets as $target)
		{
			trace("## Checking {$target->target_class} - {$target->associated_classes}", 3);
			
			$classes = explode(",", $target->associated_classes);
			if (array_search($class, $classes) !== FALSE)
			{
				trace("## Found a match", 3);
				
				$fieldRenderer = TaxonomyManager::getBindingFieldRendererClass($target);
				
				$renderer = new $fieldRenderer($form, $target);
				$renderer->setSize(400, 125);
				$renderer->resizable = true;
				$renderer->showSelectAll = $showSelectAll;
		
				if ($group)
				{
					$form->group($group, $class);
				}
		
				$renderers[] = $renderer;
			}
		}
		
		return $renderers;
	}
	
	static function getAssociatedTaxonomies($classes, $facet_filter_only = false)
	{
		if (!is_array($classes))
		{
			$classes = array($classes);
		}
		
		$matched = array();
		
		$taxonomies = Query::create(Taxonomy, "")->execute();
		
		foreach($taxonomies as $taxonomy)
		{
			if ($facet_filter_only and !$taxonomy->enable_facet_filter) continue;
			
			$taxonomy_classes = explode(",", $taxonomy->associated_classes);
			foreach($classes as $class)
			{
				if (array_search($class, $taxonomy_classes) !== FALSE)
				{
					$matched[] = $taxonomy;
					break;
				}
			}
		}	
		
		return $matched;	
	}
	
	static function getBindingTargets($classes, $facet_filter_only = false)
	{
		if (!is_array($classes))
		{
			$classes = array($classes);
		}
		
		$matched = array();
		
		$bindingTargets = Query::create(BindingTarget, "")->execute();
		
		foreach($bindingTargets as $bindingTarget)
		{
			if ($facet_filter_only and !$bindingTarget->enable_facet_filter) continue;
			
			$assoc_classes = explode(",", $bindingTarget->associated_classes);
			foreach($classes as $class)
			{
				if (array_search($class, $assoc_classes) !== FALSE)
				{
					$matched[] = $bindingTarget;
					break;
				}
			}
		}	
		
		return $matched;	
	}
	
	static function importTerms($taxonomy, $file)
	{
		$indexedTerms = reindexList($taxonomy->Terms(), "term");
		
		$fp = fopen($file, "r");
		
		$fields = fgetcsv($fp);
		
		$fileIdx = -1;
		
		foreach($fields as $field)
		{
			if ($field == "term")
			{
				$fileIdx = $i;
				break;
			}
			
			$i++;
		}

		if ($fileIdx < 0)
		{
			throw new FakoliException("'term' column not present");
		}
		
		while($values = fgetcsv($fp))
		{
			$term = $values[$fileIdx];
			
			trace("Importing term", 3);
			
			$i = 0;
					
			if (array_key_exists($term, $indexedTerms)) 
			{
				$import = $indexedTerms[$term];
				foreach($fields as $field)
				{					
					$import->set($field, $values[$i]);
					$i++;
				}
					
				$import->save();
			}
			else
			{
				$import = new TaxonomyTerm();
				foreach($fields as $field)
				{					
					$import->set($field, $values[$i]);
					$i++;
				}
			
				$import->taxonomy_id = $taxonomy->taxonomy_id;
				$import->save();
			}
		}
		
		fclose($fp);
	}
	
	static function addFacets($manager, $classes, $dropdown = true, $max_width = "200px", $width = "200px", $height = "120px")
	{
		if (!is_array($classes) && $classes) $classes = array($classes);
		
		$bindingTargets = TaxonomyManager::getBindingTargets($classes, true);
		foreach($bindingTargets as $target)
		{
			$facetFilter = TaxonomyManager::getBindingFacetFilterClass($target);
			
			$facet = $manager->addFacet($target->target_class, new $facetFilter($target));
			$facet->dropdown = $dropdown;
			$facet->dropdownMaxWidth = $max_width;
			$facet->width = $width;
			$facet->height = $height;
		}
			
		$taxonomies = TaxonomyManager::getAssociatedTaxonomies($classes, true);
		foreach($taxonomies as $taxonomy)
		{
			$facet = $manager->addFacet($taxonomy->taxonomy_name, new TaxonomyFacetFilter($taxonomy));
			$facet->dropdown = $dropdown;
			$facet->dropdownMaxWidth = $max_width;
			$facet->width = $width;
			$facet->height = $height;
		}
	}
	
	static function getRequestTerms($classes)
	{
		$ids = array();
		
		if (!is_array($classes) && $classes) $classes = array($classes);
		
		$taxonomies = TaxonomyManager::getAssociatedTaxonomies($classes, true);
		foreach($taxonomies as $taxonomy)
		{
			if (isset($_REQUEST[$taxonomy->identifier]))
			{
				$term_ids = explode(",", $_REQUEST[$taxonomy->identifier]);
				checkNumeric($term_ids);
				foreach($term_ids as $term_id)
				{
					if ($term_id) $ids[] = $term_id;
				}
			}
		}

		if (count($ids) == 0) return array();
		$idString = implode(", ", $ids);
		
		return Query::create(TaxonomyTerm, "WHERE term_id IN ($idString)")->execute();
	}
	
	static function getRequestFilterConstraint($class)
	{
		$taxonomies = TaxonomyManager::getAssociatedTaxonomies($class, true);
		
		$obj = new $class;
		$pk = $obj->getPrimaryKey();
		
		$clauses = array();
		
		foreach($taxonomies as $taxonomy)
		{
			if (isset($_REQUEST[$taxonomy->identifier]))
			{
				$term_ids = $_REQUEST[$taxonomy->identifier];
				
				if (!$term_ids) continue;
				if (!preg_match("/^[\\d,]+/", $term_ids))
				{
					throw new FakoliException("Invalid facet filter terms");
				}
				
				$clauses[] = "$pk IN (SELECT id FROM taxonomy_term_association WHERE class='$class' AND term_id IN ($term_ids))";
			}
		}
		
		return implode(" AND ", $clauses);
	}
	
	static function cloneTaxonomy($form)
	{
		$tx = new DataTransaction();
		
		try 
		{
			$clone = $form->data;
			
			$clone->joinTransaction($tx);
			
			$clone->filter = null;
			$name = $clone->taxonomy_name;
			$ident = $clone->identifier;
			
			$taxonomy_id = checkNumeric($_GET["taxonomy_id"]);
			$src = new Taxonomy($taxonomy_id);
			$clone->copy($src);
			unset($clone->taxonomy_id);
			$clone->taxonomy_name = $name;
			$clone->identifier = $ident;
			$clone->published = false;
			$clone->save();
	
			$terms = $src->Terms();

			foreach($terms as $term)
			{
				$cloneTerm = new TaxonomyTerm();
				$cloneTerm->copy($term);
				$cloneTerm->taxonomy_id = $clone->taxonomy_id;
				unset($cloneTerm->term_id);
				$cloneTerm->joinTransaction($tx);
				$cloneTerm->save();
			}
			
			$tx->commit();
			return true;
		}
		catch (Exception $e)
		{
			$tx->rollback();
			$form->msg = $e->getMessage();
			return false;
		}
	}
	
	/**
	 * Retrieve items of the specified class that are bound to the specified source object
	 * @param DataItem $source the source item
	 * @param string $class the class of bound items to retrieve
	 * @param string $constraint optional extra constraint or ordering
	 * @return array any items of the specified class that are bound to the target
	 */
	static function getBoundItems($source, $class, $constraint = "")
	{
		$constraint = preg_replace("/^\\s*WHERE/i", "AND", $constraint);
		$obj = new $class;
		$pk = $obj->getPrimaryKey();
		
		$items = Query::create($class, "WHERE $pk IN (SELECT target_id FROM binding WHERE id=:source_id and class=:source_class AND target_class=:target_class) $constraint")
					  ->bind(":source_id", $source->get($source->getPrimaryKey()),
							 ":target_class", $class,
							 ":source_class", get_class($source))
					  ->execute();
		 
		return $items;
	}

	/**
	 * Same as TaxonomyManager::getBoundItems, but with the reverse relationship
	 * @param DataItem $target the target item
	 * @param string $class the class of bound items to retrieve
	 * @param string $constraint optional extra constraint or ordering
	 * @return array any items of the specified class that are bound to the target
	 */
	static function getBoundItemsReverse($target, $class, $constraint = "")
	{
		$constraint = preg_replace("/^\\s*WHERE/i", "AND", $constraint);
		$obj = new $class;
		$pk = $obj->getPrimaryKey();
		
		$items = Query::create($class, "WHERE $pk IN (SELECT id FROM binding WHERE target_id=:target_id and class=:source_class AND target_class=:target_class) $constraint")
		->bind(":target_id", $target->get($target->getPrimaryKey()),
				":target_class", get_class($target),
				":source_class", $class)
				->execute();
			
		return $items;
		
	}
	
	/**
	 * Returns the number of items of the specified class that are bound to the specified source object
	 * @param DataItem $source the source item
	 * @param string $class the class of bound items to retrieve
	 * @param string $constraint optional extra constraint or ordering
	 * @return array any items of the specified class that are bound to the target
	 */
	static function countBoundItems($target, $class, $constraint = "")
	{
		$constraint = preg_replace("/^\\s*WHERE/i", "AND", $constraint);
		$obj = new $class;
		$pk = $obj->getPrimaryKey();
		
		$items = Query::create($class, "WHERE $pk IN (SELECT target_id FROM binding WHERE id=:source_id and class=:source_class AND target_class=:target_class) $constraint")
					  ->bind(":source_id", $source->get($source->getPrimaryKey()),
							 ":target_class", $class,
							 ":source_class", get_class($source))
					  ->executeValue("COUNT(1)");
		 
		return $items;
	}

	/**
	 * Same as TaxonomyManager::countBoundItems, but with the reverse relationship
	 * @param DataItem $target the target item
	 * @param string $class the class of bound items to retrieve
	 * @param string $constraint optional extra constraint or ordering
	 * @return array any items of the specified class that are bound to the target
	 */
	static function countBoundItemsReverse($target, $class, $constraint = "")
	{
		$constraint = preg_replace("/^\\s*WHERE/i", "AND", $constraint);
		$obj = new $class;
		$pk = $obj->getPrimaryKey();
	
		$items = Query::create($class, "WHERE $pk IN (SELECT id FROM binding WHERE target_id=:target_id and class=:source_class AND target_class=:target_class) $constraint")
		->bind(":target_id", $target->get($target->getPrimaryKey()),
				":target_class", get_class($target),
				":source_class", $class)
				->executeValue("COUNT(1)");
			
		return $items;
	
	}
	
	/**
	 * Generates a search constraint matching the supplied text against taxonomy terms that might be associated
	 * with the given class
	 * @param string $text the search text 
	 * @param string $class the DataItem class that is being search
	 * @param string $placeholder true if $text is a placeholder variable, false, 
	 * @return string a SQL constraint that can be used to perform the taxonomy text match
	 */
	static function generateSearchConstraint($text, $class, $placeholder = false)
	{		
		$obj = new $class;
		$pk = $obj->getPrimaryKey();
		$text = $placeholder ? $text : "'$text'";
		
		return "$pk in (select id from taxonomy_term_association a, taxonomy_term t where t.term like {$text} and a.term_id=t.term_id and a.class='{$class}')";
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new TaxonomyUpgradeManager();
		$mgr->upgrade($version);
	}	
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("taxonomy", "Taxonomies and Terms", new TaxonomySerializationHandler());
		SerializationManager::registerHandler("taxonomy_associations", "Taxonomy Term Associations", new SimpleSerializationHandler(TaxonomyTermAssociation));
		return true;
	}
}

?>