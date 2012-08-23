-- Fakoli Survey Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `survey` (
  `survey_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(100) NOT NULL,
  `introduction` text COMMENT 'instructions to users completing survey',
  `start_date` date DEFAULT NULL COMMENT 'date survey request sent',
  `end_date` date DEFAULT NULL COMMENT 'date survey is closed',
  `user_id` int(10) unsigned NOT NULL COMMENT 'references site_user table',
  `sender_email` varchar(100) DEFAULT NULL,
  `recipients` text,
  `message` text,
  `status` varchar(15) NOT NULL DEFAULT 'not sent',
  `deleted` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`survey_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Links a questionnaire to an email template';

CREATE TABLE `survey_answer` (
  `survey_answer_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `response_id` int(10) unsigned NOT NULL COMMENT 'references questionnaire_response table',
  `survey_question_id` int(10) unsigned NOT NULL COMMENT 'references single question in a questionnaire table',
  `value` text,
  PRIMARY KEY (`survey_answer_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Answer to a question';

CREATE TABLE `survey_question` (
  `survey_question_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `question_type_id` int(10) unsigned NOT NULL COMMENT 'freetext, multiple choice, ratings, etc.',
  `question` text NOT NULL COMMENT 'the text of the question',
  `question_name` varchar(80) DEFAULT NULL COMMENT 'short question title, for spreadsheet view',
  `options` text COMMENT 'for checklists or multiple choice, the list of answer options',
  `required` tinyint(3) unsigned DEFAULT '0' COMMENT '1 for required text; count for required number of checks in checkbox list',
  `char_limit` int(8) unsigned DEFAULT '0' COMMENT 'character limit for free text fields',
  `num_rows` tinyint(3) unsigned DEFAULT '0' COMMENT 'number of input rows to display on freetext',
  `locked` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'whether this is a standard, multi-reuse question',
  PRIMARY KEY (`survey_question_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='questions for questionnaires: surveys, applications, etc';


CREATE TABLE `survey_question_xref` (
  `survey_question_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `survey_id` int(10) unsigned DEFAULT NULL COMMENT 'if survey question set',
  `survey_template_id` int(10) unsigned DEFAULT NULL COMMENT 'if standard question set',
  `survey_question_id` int(10) unsigned NOT NULL,
  `sort_order` tinyint(4) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`survey_question_xref_id`),
  KEY `survey_idx` (`survey_id`),
  KEY `survey_question_idx` (`survey_question_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='survey question set or standard set';

CREATE TABLE `survey_response` (
  `response_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `survey_id` int(10) unsigned NOT NULL COMMENT 'references questionnaire table',
  `token` varchar(10) NOT NULL COMMENT 'assigned response token',
  `email` varchar(100) NOT NULL,
  `last_modified` date DEFAULT NULL COMMENT 'date submitted',
  `status` varchar(20) NOT NULL DEFAULT 'not_started',
  PRIMARY KEY (`response_id`),
  UNIQUE KEY `token_idx` (`token`,`survey_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


INSERT INTO `merge_code` (`name`,`description`,`map`,`class_name`) VALUES
 ('survey_link',NULL,'getEditUrl','SurveyResponse'),
 ('survey_intro_link',NULL,'getIntroUrl','SurveyResponse'),
 ('token',NULL,'SurveyResponse.token','SurveyResponse'),
 ('survey_title',NULL,'Survey.title','SurveyResponse'),
 ('survey_end_date',NULL,'Survey.end_date','SurveyResponse');



-- END Version 1.0

-- START Version 1.1

DROP TABLE IF EXISTS `survey_template`;

ALTER TABLE `survey_question_xref` drop column `survey_template_id`;

ALTER TABLE `survey_question` drop column `locked`;

ALTER TABLE `survey_response` CHANGE COLUMN `status` `status` varchar(20) NOT NULL DEFAULT 'not_started';
update `survey_response` set status = 'not_started' where status = '0';
update `survey_response` set status = 'in_progress' where status = '1';
update `survey_response` set status = 'submitted' where status = '2';

-- END Version 1.1


-- START Version 1.2

ALTER TABLE `survey_question` ADD COLUMN `context_help` TEXT;

-- END Version 1.2

-- START Version 1.3

ALTER TABLE `survey` ADD COLUMN `confirmation_message` TEXT after `message`;

-- END Version 1.3

-- START Version 1.4

ALTER TABLE `survey` ADD COLUMN `allow_anonymous_responses` TINYINT(3) DEFAULT 0;

-- END Version 1.4

-- START Version 1.5

ALTER TABLE `survey` ADD COLUMN `instructions` TEXT AFTER `end_date`;

-- END Version 1.5

-- START Version 1.6

ALTER TABLE `survey` ADD COLUMN `show_preview_before_submitting` TINYINT(3) DEFAULT 1;

-- END Version 1.6