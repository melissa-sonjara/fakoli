-- Fakoli Questionnaire Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `answer` (
  `answer_id` int(10) unsigned NOT NULL auto_increment,
   `question_id` int(10) unsigned NOT NULL COMMENT 'references single question in a questionnaire table',
  `value` text,
`composite_class` varchar(100) default NULL,
  PRIMARY KEY  (`answer_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Answer to a question';

CREATE TABLE `question` (
  `question_id` int(10) unsigned NOT NULL auto_increment,
  `questionnaire_id` int(10) unsigned NOT NULL COMMENT 'references questionnaire table',
  `question_number` int(10) unsigned NOT NULL,
  `question_type_id` int(10) unsigned NOT NULL COMMENT 'freetext, multiple choice, ratings, etc.',
  `question` text NOT NULL COMMENT 'the text of the question',
  `question_name` varchar(80) default NULL COMMENT 'short question title, for spreadsheet view',
  `options` text COMMENT 'for checklists or multiple choice, the list of answer options',
  `required` tinyint(3) unsigned default '0' COMMENT '1 for required text; count for required number of checks in checkbox list',
  `char_limit` int(8) unsigned default '0' COMMENT 'character limit for free text fields',
  `num_rows` tinyint(3) unsigned default '0' COMMENT 'number of input rows to display on freetext',
  PRIMARY KEY  (`question_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='questions for questionnaires: surveys, applications, etc';

CREATE TABLE `question_type` (
  `question_type_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL COMMENT 'type name',
  `class_name` varchar(100) NOT NULL COMMENT 'the name of the question renderer class',
  `sort_order` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'order in the question form drop down',
  `options` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'whether this type has an options list',
  `char_limit` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'whether this type can have a char limit; 0 if not',
  `num_rows` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'default number of rows, if applicable to type; 0 if not',
  PRIMARY KEY (`question_type_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Question types available for questionnaires';

-- Default question types
/*!40000 ALTER TABLE `question_type` DISABLE KEYS */;
INSERT INTO `question_type` (`question_type_id`,`name`,`class_name`,`sort_order`,`options`,`char_limit`,`num_rows`) VALUES
 (1,'Multiple Choice','MultipleChoiceView',4,1,0,0),
 (2,'Rating Question','RatingView',5,1,0,0),
 (3,'Short Text','ShortTextView',2,0,0,0),
 (4,'Free Text','FreeTextView',1,0,1,3),
 (5,'Checklist','CheckListView',3,1,0,0),
 (6,'Drop Down List','SelectFieldView',6,1,0,0);
/*!40000 ALTER TABLE `question_type` ENABLE KEYS */;

CREATE TABLE  `questionnaire` (
  `questionnaire_id` int(10) unsigned NOT NULL auto_increment,
  `title` varchar(100) NOT NULL COMMENT 'title of the individiual questionnaire: e.g., Course 312 Evaluation',
  `composite_class` varchar(100) default NULL,
  PRIMARY KEY  (`questionnaire_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='user-defined questionnaire';

-- END Version 1.0
