<?php
/**\addtogroup questionnaire
 * @{
 */


Fakoli::using("questionnaire", "text_lookup");
Fakoli::usingFile("/cms/components/questionnaire/abstract_questionnaire_manager.inc");

/*
 * Provides an interface between a custom questionnaire
* component and the QuestionnaireForm and Question Renderers
* and Results View classes. Revies an object of class
* Question or Survey or other DataItem class that can
* access the questions and answers.
*
* This class defines the methods that are required for components that
* use the QuestionnaireForm for the user survey-create interface.
*
* Instantiating this class requires the questionnaire or
* survey object.
*/
abstract class AbstractQuestionnaireCreateManager  extends AbstractQuestionnaireManager
{
	var $item;				//<	The questionnaire or survey object
	var $validation_msg;

	function AbstractQuestionnaireCreateManager($item)
	{
		$this->item = $item;
	}

	/**
	 * The name of the questionnaire/survey component
	 */
	abstract function getComponentName();

	/**
	 * The DataItem class name for the Question class
	*/
	abstract function getQuestionClass();

	/**
	 * If questions are linked to the questionnaire/survey through an xref
	 *
	 * Implementations that link questions directly to the questionnaire/survey
	 * will return null.
	*/
	abstract function getXrefClass();

	/**
	 * The title field used for the clone dialog.
	*/
	function getTitleField()
	{
		return "title";
	}

	/**
	 * Called by QuestionnaireForm function getIndexedAnswers
	 *
	 * The CreateManager should return null because this manager is used
	 * to display a preview of the questionnaire the user created, not
	 * the answers. The answers will not be display so it would be
	 * a wasted query to retrieve them.
	 */
	function getAnswers()
	{
		return null;
	}

	function getQuestionDeleteHandler()
	{
		return "question_delete";
	}

	function getQuestionRemoveHandler()
	{
		return "question_remove";
	}

	function getReorderHandler()
	{
		return "reorder_questions";
	}
	
	function getClassName()
	{
		return $this->item->prettifyClassName();
	}

	/**
	 * Creates a default name for the questionnaire list page
	 * from the item table name.
	 *
	 * E.g., table name survey results in list page
	 * survey_questions
	 */
	function getQuestionListIdentifier()
	{
		return "{$this->item->table}_questions";
	}

	/**
	 * Creates a default name for the questionnaire question form
	 * page.
	 * e.g., questionnaire_question_form
	 */
	function getQuestionForm()
	{
		return "{$this->item->table}_question_form";
	}

	/**
	 * Creates a default name for the questionnaire form
	 * or detail first tab from the item table name.
	 *
	 * e.g., questionnaire_form
	 */
	function getQuestionnaireFormIdentifier()
	{
		if(!$this->item) 
		{
			return;
		}
		return "{$this->item->table}_form";
	}

	/**
	 * From question list page, format the link to edit the question.
	 *
	 * @param $question
	 */
	function formatQuestionLink($question)
	{
		$question_form_id = $this->getQuestionForm();
		$qPk = $question->getPrimaryKey();

		return $question->format("<a href='{$question_form_id}?{$qPk}={{$qPk}}'>{question}</a>\n");
	}

	/**
	 * Copies the manager instance's questionnaire item from destination (dst).
	 *
	 * @param Array $copyFields - the array of fields of the Questionnaire
	 * item that should be copied (e.g., title)
	 *
	 */
	function cloneQuestionnaire($title_field, $cloneFields = array())
	{
		$src = $this->item;
		$itemClass = get_class($src);
		$dst = new $itemClass();

		if(is_callable(array($dst, setDefaults)))
		{
			$dst->setDefaults();
		}
		$dst->$title_field = $src->$title_field;

		if(count($cloneFields) > 0)
		{
			foreach($cloneFields as $field)
			{
				$dst->$field = $src->$field;
			}
		}
			
		$dst->save();

		$questions = $this->getQuestions();
		if(count($questions) == 0) return;

		$xref_class = $this->getXrefClass();	
		$sort_field = $this->getSortField();
		$sort_order = 1;
		
		$qPk = $questions[0]->getPrimaryKey();
		$itemPk = $dst->getPrimaryKey();
			
		/**
		 * If questionnaire uses xref, save new xref record linking
		 * it to the question; otherwise, save question record.
		 */
		foreach($questions as $question)
		{
			$obj = ($xref_class) ? new $xref_class() : clone $question;
			$obj->$qPk = ($xref_class) ? $question->$qPk : null;
			if($xref_class)
			{
				$obj->$sort_field = $sort_order;
			}
			$obj->$itemPk = $dst->$itemPk;
			$obj->save();
			$sort_order++;
		}
				
		return $dst;
	}

	/**
	 * Delete a questionnaire item and either all its questions
	 * or all its question xrefs.
	 */
	function deleteQuestionnaire()
	{
		$item = $this->item;

		if(!$item->allowDelete())
		{
			return false;
		}
		$itemPk = $item->getPrimaryKey();
		$obj = $this->getSortableObj();
	
		$questions = $this->getQuestions();
		$qPk = $this->getQuestionKey();

		$tx = new DataTransaction();

		try
		{
			if(count($questions) > 0)
			{
				// Either deleting all questions linked to the questionnaire or all
				// question xrefs linked to the questionnaire
				$obj->delete("WHERE {$itemPk}={$item->$itemPk}");
				foreach($questions as $question)
				{
					$question->joinTransaction($tx);
					$this->deleteOrphanQuestion($question);
				}
			}
			$item->joinTransaction($tx);
			$item->delete();
			$tx->commit();
		}
		catch(Exception $e)
		{
			Fakoli::end($e->getMessage());
		}

		return true;
	}

	function logicalDeleteQuestionnaire()
	{
		$this->item->filter = null;
		
		if($this->item->hasField("deleted"))
		{
			$this->item->deleted = true;
			$this->item->filter = new InclusionFilter("deleted");
			$this->item->save();
		}
	}


	/**
	 * Used when the question is linked through an xref table to the
	 * questionnaire/survey obj
	 *
	 * @param $xref obj of xref class that links questions to questionnaire/survey
	 */
	function removeQuestionXref($xref_id)
	{
		$item = $this->item;
		$itemPk = $item->getPrimaryKey();
		$xref_class = $this->getXrefClass();

		try
		{
			$xref = new $xref_class($xref_id);
			$xrefPk = $xref->getPrimaryKey();
				
			if ($item->$itemPk != $xref->$itemPk)
			{
				throw new FakoliException("QuestionnaireManager removeQuestion Data Mismatch");
			}
			$xref->delete("WHERE $xrefPk={$xref_id}");
			$this->reNumberQuestions();
		}
		catch(Exception $e)
		{
			Fakoli::end($e->getMessage());
		}

		$qPk = $this->getQuestionKey();
		$qClass = $this->getQuestionClass();
		$question = new $qClass($xref->$qPk);
		$this->deleteOrphanQuestion($question);
	}

	/**
	 * Used when the question is linked through an xref table to the
	 * questionnaire/survey obj
	 *
	 * @param $question_id
	 */
	function removeQuestion($question_id)
	{
		$item = $this->item;
		$itemPk = $item->getPrimaryKey();
		$qClass = $this->getQuestionClass();
		$question = new $qClass($question_id);
		$qPk = $question->getPrimaryKey();
		$xref = $this->getSortableObj();

		try
		{
			$xref->delete("WHERE {$itemPk}={$item->$itemPk} AND {$qPk}={$question->$qpK}");
			$this->reNumberQuestions();
		}
		catch(Exception $e)
		{
			Fakoli::end($e->getMessage());
		}

		$this->deleteOrphanQuestion($question);
	}

	/**
	 * Called from question form. On save question, save the
	 * question xref record.
	 *
	 * @param $question
	 * @param $sort_order - has a value if adding multiple questions, called from
	 * saveSelectedQuestions
	 */
	function saveQuestionXref($question, $sort_order = 0)
	{
		$item = $this->item;
		$xref_class = $this->getXrefClass();
		$itemPk = $this->getPrimaryKey();
		$qPk = $question->getPrimaryKey();

		if(!$xref_class)
		{
			return;
		}

		$xref = new $xref_class();
			
		if($xref->exists("WHERE {$itemPk}={$item->$itemPk} AND {$qPk}={$question->$qPk}"))
		{
			return;
		}

		$xref->$itemPk = $item->$itemPk;
		$xref->sort_order = (!$sort_order) ? $this->getNextSortOrder() : $sort_order;
		$xref->$qPk = $question->$qPk;
		$xref->save();
	}

	/**
	 * If the question xref that was deleted was the only one linked to the
	 * question, then delete the question.
	 *
	 * @param $question
	 */
	function deleteOrphanQuestion($question)
	{
		$xrefClass = $this->getXrefClass();
		$qPk = $this->getQuestionKey();

		if(!$xrefClass)
		{
			return;
		}

		$count = Query::create($xrefClass, "WHERE {$qPk}=:{$qPk}")
			->bind(":{$qPk}", $question->$qPk)
			->executeValue("COUNT(1)");

		if($count == 0)
		{
			$question->delete();
		}
	}

	/**
	 * Used when the question obj is linked directly to the
	 * questionnaire/survey item.
	 *
	 * Called from delete_question action handler
	 *
	 * @param $question_id
	 */
	function deleteQuestion($question_id)
	{
		$item = $this->item;
		$qClass = $this->getQuestionClass();
		$itemPk = $item->getPrimaryKey();
		$question = new $qClass($question_id);
		$qPk = $question->getPrimaryKey();
		$item_class = $this->item->prettifyClassName();
		$question_class = $question->prettifyClassName();

		if(!$question->allowDelete())
		{
			Fakoli::end("Delete not permitted for question id {$question->$qPk} {$question->question}");
		}

		try
		{
			if ($item->$itemPk != $question->$itemPk)
			{
				throw new FakoliException("QuestionnaireManager deleteQuestion Data Mismatch $item_class {$itemPk} {$item->$itemPk} does not match question $question_class {$itemPk} {$question->$itemPk} {$qPk} {$question->$qPk}");
			}
				
			$question->delete();
			$this->reNumberQuestions();
		}
		catch(Exception $e)
		{
			Fakoli::end($e->getMessage());
		}

	}


	/**
	 * Reorder and renumber questions on save question list page.
	 *
	 * The function expects that the sort order input field be labeled "question_{question_id}"
	 * as is the case if QuestionTableHelper formatSortOrder is used to create the field.
	 *
	 * The function expects that the sort order field is contained in the Question class
	 * and is either named "sort_order" or "question_number".
	 */
	function reOrderQuestions()
	{
		global $_POST;

		$item = $this->item;
		$sortableClass = $this->getSortableClass();
		$sortableObj = new $sortableClass();
		$pk = $sortableObj->getPrimaryKey();
		$itemPk = $item->getPrimaryKey();
		$sort_field = $this->getSortField();
			
		// reorder
		foreach($_POST as $name => $sort_order)
		{
			if (!strncmp($name, "question_", 9))
			{
				$id = substr($name, 9);
				checkNumeric($id);
				checkNumeric($sort_order);

				$sortableObj = new $sortableClass($id);
				$sortableObj->filter = new InclusionFilter($pk, $sort_field);
				$sortableObj->$itemPk = $item->$itemPk;
				$sortableObj->$pk = $id;
				$sortableObj->$sort_field = $sort_order;
				$sortableObj->save();
			}
		}

		$this->reNumberQuestions();
	}


	/**
	 * Instantiates the xref class if set or question class if not
	 * to sort the items.
	 *
	 * Retrieves the sortable objs through the function getSortableItems
	 * which expects that the mgr's item has the relation of pluralized
	 * xref class or pluralized question class
	 *
	 */
	function reNumberQuestions()
	{
		$item = $this->item;
		$sortableClass = $this->getSortableClass();
		$sortableObj = new $sortableClass();
		$pk = $sortableObj->getPrimaryKey();
		$sort_field = $this->getSortField();

		$objs = $this->getSortableItems();
		$sort_order = 1;

		foreach($objs as $obj)
		{
			$obj->filter = new InclusionFilter($sort_field);
			$obj->$sort_field = $sort_order;
			$obj->save();
			$sort_order++;
		}
	}


	/**
	 * Used on question select page
	 * 
	 * Retrieve all the question sets for all surveys/questionnaires
	 * but exclude any questions already linked to the given
	 * item.
	 * 
	 * returns array of questions not already in the given survey
	 */
	function getQuestionSets()
	{
		$itemPk = $this->item->getPrimaryKey();
		$qPk = $this->getQuestionKey();
		$xref = $this->getSortableObj();
		$xref_class = get_class($xref);

		if(!$xref)
		{
			return;
		}

		$query = GroupedQuery::create($xref_class, "", "{$itemPk}");
		if($this->item->$itemPk)
		{
			$query->constraints = "WHERE {$qPk} NOT IN (SELECT {$qPk} FROM {$xref->table} WHERE {$itemPk} = :{$itemPk})";
			$query->bind(":{$itemPk}", $this->item->$itemPk);
		}

		return $query->execute();
	}

	/**
	 * Called from question select page for components that link questions
	 * through an xref
	 *
	 * @param Array $items questionnaires/surveys user can select from. Components
	 * may have criteria to include/exclude certain records; array must beindexed
	 * by primary key
	 * 
	 * ex: $items = IndexedQuery::create(Survey, "WHERE deleted=false", "survey_id")->execute();
	 * 
	 * Implementations may also restrict based on survey ownership.
	 *
	 * returns instance of AutoForm
	 */
	function buildQuestionSelectForm($items = null)
	{
		$class = $this->getClassName();
		$this->item->filter = new InclusionFilter();
		$form = new AutoForm($this->item);
		$itemPk = $this->getPrimaryKey();
		$title_field = $this->getTitleField();
		
		if(!$items)
		{
			$items = $this->getQuestionnaires();
		}

		$questionSets = $this->getQuestionSets();

		$table = new GroupedDataListView($questionSets, "QuestionSets");
		$table->mode = "fixed";
		$table->selector();
		$table->column("Question", "{Question.question}", false)
			->column("Options", array(QuestionTableHelper, formatOptions), false)
			->column("Required", array(QuestionTableHelper, formatRequired), false, "text-align: center")
			;
		
		$table->emptyMessage = "There are no $class questions to select.";

		foreach(array_keys($questionSets) as $item_id)
		{
			if(!array_key_exists($item_id, $items)) continue;

			$item = $items[$item_id];
			$table->group($item->$title_field, $item_id);
		}

		$instructions = TextLookup::getText("question_select_instructions");
		$questionSelect = new DataListFieldRenderer($form, $table, "survey_question_select", "<h3>Select Survey Questions</h3>$instructions");
		$questionSelect->hideLabel = true;
		$form->submitLabel = "Select";
		$form->buttons_at_top = true;

		if(!count($questionSets))
		{
			$form->readOnlyForm = true;
		}

		$questionSelect->onPostProcess = array($this, saveSelectedQuestions);

		return $form;
	}

	/*
	 * Called by DataListFieldRenderer onPostProcess
	* from the question select page.
	*
	* Save questions selected
	*
	* It is possible for the user to select the same question
	* if that question is part of 2 or more surveys.
	*/
	function saveSelectedQuestions($questionSelect, $field)
	{
		global $_POST;

		$xref_class = $this->getXrefClass();
		if(!$xref_class)
		{
			return;
		}

		$xref = new $xref_class();
		$xrefPk = $xref->getPrimaryKey();
		$questionClass = $this->getQuestionClass();
		$qPk = $this->getQuestionKey();

		$questionIds = $_POST[$xrefPk];

		if(isset($questionIds) AND count($questionIds) > 0)
		{
			$list = implode(",", array_values($questionIds));
			// Given the set of xref ids selected, retrieve the questions from the question table
			$questions = Query::create($questionClass, "WHERE {$qPk} IN (SELECT {$qPk} FROM {$xref->table} WHERE {$xrefPk} IN ($list))")
			->execute();

			if(count($questions) > 0)
			{
				$sort_order = $this->getNextSortOrder();

				foreach($questions as $question)
				{
					$this->saveQuestionXref($question, $sort_order);
				}
			}
		}
		return true;
	}

	function filterActions(&$actions)
	{
		if(!count($actions)) return;
		$item = $this->item;

		if(!is_callable(array($item, isAuthor))) return;

		if(!$item->isAuthor())
		{
			unset($actions["edit"], $actions["send"], $actions["open"],
					$actions["close"], $actions["send_reminders"], $actions["reopen"],
					$actions["share"], $actions["unshare"], $actions["delete"], $actions["send_test"]);
		}
	}

	/**
	 * From the question list page, set the table to allow draggable reorder.
	 *
	 * @param $table
	 */
	function enableDragReorder(&$table)
	{
		$component_name = $this->getComponentName();
		$reorder_handler = $this->getReorderHandler();
		$itemPk = $this->getPrimaryKey();

		$table->enableDragReorder("/action/{$component_name}/{$reorder_handler}?{$itemPk}={$this->item->$itemPk}");
		$table->dragText = "<span style='font-size: 10px'>Click and drag to change the order of the questions</span>";
	}

	/**
	 * Called from reorder_questions action handler which
	 * responds to draggable reordering in data list view.
	 *
	 * @param String $orderClass - name of the DataItem class that stores the
	 * question order (e.g., Question, or SurveyQuestionXrefQuestion)
	 * This class must have a field named "sort_order" or "question_number"
	 * and have a field that matches the item's pk field (e.g., questionnaire_id or
	 * survey_id)
	 */
	function saveDraggableQuestionOrder()
	{
		$item = $this->item;
		$itemPk = $item->getPrimaryKey();
		$sortableClass = $this->getSortableClass();
		$sortableObj = new $sortableClass();
		$sort_field = $this->getSortField();
			
		$pk = $sortableObj->getPrimaryKey();
		$tx = new DataTransaction();

		try
		{
			foreach($_GET[$pk] as $id => $sort_order)
			{
				checkNumeric($id);
				checkNumeric($sort_order);

				$sortableObj = new $sortableClass();
				$sortableObj->joinTransaction($tx);
					
				$sortableObj->load($id);
					
				if ($item->$itemPk != $sortableObj->$itemPk)
				{
					throw new FakoliException("Data Mismatch");
				}

				$sortableObj->filter = new InclusionFilter($pk, $sort_field);
				$sortableObj->$sort_field = $sort_order;
				$sortableObj->save();
			}

			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			Fakoli::end($e->getMessage());
		}
	}

	/**
	 * Get the next sort order for a new question.
	 *
	 * If the questions are linked to the questionnaire directly, then
	 * query the Question class.
	 *
	 * If the questions are linked through an xref, then query the
	 * xref class.
	 */
	function getNextSortOrder()
	{
		$item = $this->item;
		$sortableClass = $this->getSortableClass();
		$itemPk = $item->getPrimaryKey();
		$sort_field = $this->getSortField();
			
		return Query::create($sortableClass, "WHERE {$itemPk}=:{$itemPk}")
		->bind(":{$itemPk}", $item->$itemPk)
		->executeValue("MAX($sort_field)") + 1;
	}

	function validateQuestionnaire()
	{
		$pk = $this->getPrimaryKey();
		$id = $this->item->$pk;

		if(count($this->getQuestions()) == 0)
		{
			$identifier = $this->getQuestionListIdentifier();
			$msg = "Your <a href='{$identifier}?$pk=$id'>questionnaire</a> must have at least one question.";
		}

		return $msg;
	}


	function writeQuestionnaireValidationMsg()
	{
		$msg = $this->validateQuestionnaire();
		echo "<div id='warning'>{$msg}</div>\n";
	}
	
	/**
	 * Allow managers to override the default questionnaire mgr name -
	 * needed if multiple managers would be on same page.
	 * 
	 * @return string
	 */
	function getJSManagerName()
	{
		return "questionnaireMgr";
	}

	/**
	 * Instantiate the QuestionnaireCreateManager js class.
	 *
	 * Requires the following handlers to exist in the questionnaire component.
	 * The item class is used for the error tag for dialog results js function.
	 *
	 * Default question handler names:
	 * question_delete - used when no xref
	 * question_remove - used when questions linked through xref
	 * reorder_questions
	 *
	 */
	function writeScript()
	{
		$item = $this->item;
		$qPk = $this->getQuestionKey();
		$itemPk = $item->getPrimaryKey();
		$item_id = $item->$itemPk;
		$component_name = $this->getComponentName();
		$question_delete_handler = $this->getQuestionDeleteHandler();
		$question_remove_handler = $this->getQuestionRemoveHandler();
		$question_list_identifier = $this->getQuestionListIdentifier();
		$mgrName = $this->getJSManagerName();

		$xrefPk = "";
		$xref = $this->getSortableObj();
		if($xref)
		{
			$xrefPk = $xref->getPrimaryKey();
		}

		ob_start();
		?>
<script type="text/javascript" src="/components/questionnaire/js/questionnaire_create.js"></script>
<script type="text/javascript">
var <?php echo $mgrName ?>;

window.addEvent('domready', function()
{
	<?php echo $mgrName ?> = new QuestionnaireCreateManager(
			'<?php echo $qPk ?>',
			 '<?php echo $itemPk ?>',
			<?php echo $item_id ?>,
			'<?php echo $xrefPk ?>',
			'<?php echo $component_name ?>',
			'<?php echo $question_delete_handler ?>',
			'<?php echo $question_remove_handler ?>',
			'<?php echo $question_list_identifier ?>'
			);
});

</script>
<?
		$script .= ob_get_contents();
		ob_end_clean();
		
		return $script;
	}	
	
} // end AbstractQuestionnaireCreateManager

/**@}*/?>