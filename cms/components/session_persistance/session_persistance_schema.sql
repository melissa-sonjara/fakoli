-- Fakoli Session Persistance Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `php_session` (
  `session_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `data` longtext,
  `expires` int(10) unsigned NOT NULL,
  `id` varchar(200) NOT NULL,
  PRIMARY KEY (`session_id`),
  KEY `session_idx` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- END Version 1.0
