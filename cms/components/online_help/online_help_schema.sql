
-- START Version 1.0

CREATE TABLE `help_book` (
  `help_book_id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
  `title` VARCHAR(200) NOT NULL,
  `identifier` varchar(200) NOT NULL,
  `description` TEXT NOT NULL,
  `remote` TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `url` VARCHAR(400),
  `roles` VARCHAR(400),
  `permissions` VARCHAR(400),
  PRIMARY KEY (`help_book_id`)
)
ENGINE = InnoDB;

CREATE TABLE `help_page` (
  `help_page_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `help_book_id` int(10) unsigned not null,
  `parent_id` int(10) unsigned NOT NULL,
  `title` varchar(200) DEFAULT NULL,
  `content` text NOT NULL,
  `identifier` varchar(200) NOT NULL,
  `sort_order` int(10) unsigned NOT NULL,
  `public` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY (`help_page_id`)
) 
ENGINE=InnoDB;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE `help_book` ADD COLUMN `additional_css_files` VARCHAR(1000) NOT NULL DEFAULT '';

-- END Version 1.1

-- START Version 1.2
-- END Version 1.2

-- START Version 1.3

ALTER TABLE help_book ADD COLUMN searchable TINYINT(3) NOT NULL DEFAULT 0;

-- END Version 1.3