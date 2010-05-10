-- Questionnaire sql
--
-- SQL to create tables for questionnaire component
--
-- Janice Gallant for Sonjara, Inc.
--
-- 4/15/10
--
-- ------------------------------------------------------
-- Server version	5.0.67-community-nt


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;




--
-- Definition of table `answer`
--

DROP TABLE IF EXISTS `answer`;
CREATE TABLE `answer` (
  `answer_key` int(10) unsigned NOT NULL auto_increment,
  `container_key` int(10) unsigned NOT NULL COMMENT 'references container class table, e.g., application',
  `class_key` int(10) unsigned NOT NULL COMMENT 'references dataitem_class table, identifies the container class',
  `question_key` int(10) unsigned NOT NULL COMMENT 'references single question in a questionnaire table',
  `value` text,
  PRIMARY KEY   (`answer_key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Answer to a question';




--
-- Definition of table `dataitem_class`
--

DROP TABLE IF EXISTS `dataitem_class`;
CREATE TABLE `dataitem_class` (
  `class_key` int(10) unsigned NOT NULL auto_increment,
  `class_name` varchar(50) NOT NULL COMMENT 'dataitem class name',
  PRIMARY KEY  (`class_key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;



--
-- Definition of table `question`
--

DROP TABLE IF EXISTS `question`;
CREATE TABLE `question` (
  `question_key` int(10) unsigned NOT NULL auto_increment,
  `questionnaire_key` int(10) unsigned NOT NULL COMMENT 'references questionnaire table',
  `question_number` int(10) unsigned NOT NULL,
  `question_type` int(10) unsigned NOT NULL COMMENT 'freetext, multiple choice, ratings, etc.',
  `question` text NOT NULL COMMENT 'the text of the question',
  `options` text COMMENT 'for checklists or multiple choice, the list of answer options',
  `required` tinyint(3) unsigned default '0' COMMENT '1 for required text; count for required number of checks in checkbox list',
  `char_limit` int(8) unsigned default '0' COMMENT 'character limit for free text fields',
  `num_rows` tinyint(3) unsigned default '0' COMMENT 'number of input rows to display on freetext',
  PRIMARY KEY   (`question_key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='questions for questionnaires: surveys, applications, etc';


--
-- Definition of table `questionnaire`
--

DROP TABLE IF EXISTS `questionnaire`;
CREATE TABLE `questionnaire` (
  `questionnaire_key` int(10) unsigned NOT NULL auto_increment,
  `title` varchar(100) NOT NULL COMMENT 'title of the individiual questionnaire: e.g., Course 312 Evaluation',
  `class_key` int(10) unsigned NOT NULL COMMENT 'references dataitem_class table',
  PRIMARY KEY  (`questionnaire_key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='links a questionnaire to its container class';




/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;