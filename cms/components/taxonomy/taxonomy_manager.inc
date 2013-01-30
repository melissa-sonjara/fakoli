<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

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


class TaxonomySerializationHandler
{	
	function export()
	{
		$xml  = "\n<TaxonomyMap>";
		$xml .= SerializationManager::serialize(Taxonomy, "ORDER BY taxonomy_id");
		$xml .= SerializationManager::serialize(TaxonomyTerm, "ORDER BY term_id");
		$xml .= SerializationManager::serialize(TaxonomyTermAssociation, "ORDER BY term_association_id");
		$xml .= "</TaxonomyMap>";
		
		return $xml;
	}
	
	function import($doc, $tx)
	{
		SerializationManager::unserialize(Taxonomy, $doc, $tx);
		SerializationManager::unserialize(TaxonomyTerm, $doc, $tx);	
		SerializationManager::unserialize(TaxonomyTermAssociation, $doc, $tx);		
	}		
}

/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the taxonomy component.
 */
class TaxonomyManager
{
	function TaxonomyManager()
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
	
	static function getRegisteredClasses()
	{
		$registeredClasses = array();
		
		$registeredClasses = ComponentManager::fireEvent("RegisterTaxonomyClasses", $registeredClasses);
		
		return $registeredClasses;
	}
	
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
	
	static function addTaxonomyFieldRenderers($form)
	{
		$class = get_class($form->data);
		
		$taxonomies = Query::create(Taxonomy, "")->execute();
		
		foreach($taxonomies as $taxonomy)
		{
			$classes = explode(",", $taxonomy->associated_classes);
			if (array_search($class, $classes) !== FALSE)
			{
				$renderer = new TaxonomyTermFieldRenderer($form, $taxonomy);
				$renderer->setSize(400, 125);
				$renderer->resizable = true;
			}
		}
	}
	
	static function getAssociatedTaxonomies($classes)
	{
		if (!is_array($classes))
		{
			$classes = array($classes);
		}
		
		$matched = array();
		
		$taxonomies = Query::create(Taxonomy, "")->execute();
		
		foreach($taxonomies as $taxonomy)
		{
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
	
	static function addFacets($manager)
	{
		$classes = array();
		for($i = 1; $i < func_num_args(); ++$i)
		{
			$classes[] = func_get_arg($i);
		}
		
		$taxonomies = TaxonomyManager::getAssociatedTaxonomies($classes);
		foreach($taxonomies as $taxonomy)
		{
			$manager->addFacet($taxonomy->taxonomy_name, new TaxonomyFacetFilter($taxonomy));
		}
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new TaxonomyUpgradeManager();
		$mgr->upgrade($version);
	}	
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("taxonomy", "Taxonomies and Terms", new TaxonomySerializationHandler());
		return true;
	}
}

?>