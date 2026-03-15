# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Fakoli is a PHP-based CMS framework developed by Sonjara, Inc. It uses a custom ORM, an event-driven component system, and auto-generated forms/views. The active branch `fix/php8_compatibility` is working through PHP 8 compatibility fixes.

## No Build/Test System

There is no `composer.json`, `package.json`, `Makefile`, or test framework. Development is done by deploying to a web server (Apache or IIS) and testing through a browser. Configuration is template-based — `include/config.inc` uses `%{PLACEHOLDER}` tokens that must be filled in per-environment.

## Architecture

### Request Flow

- `page.php` — public page requests, dispatches via section/identifier routing
- `action.php` — component action dispatcher via `ComponentManager`
- `resource.php` — serves framework and component static assets
- `admin/page.php` — administrative interface

### Core Abstractions

**`Fakoli`** (`cms/core.inc`) — main orchestrator. Use `Fakoli::using('component_name')` to load a component. Provides event firing, request/response handling, and storage/caching.

**`DataItem`** (`framework/data_item.inc`) — ORM base class. Subclasses declare `$table`, `$fields` (type-mapped), and relations. Supported field types: `String`, `Number`, `Date`, `Currency`, `Boolean`, `HTML`, `Text`, `Timestamp`, `File`, `Color`, etc.

**`ComponentManager`** (`cms/components/component/component_manager.inc`) — loads 77+ components, dispatches events, manages component lifecycle.

**`AutoForm`** (`framework/auto_form.inc`) — generates HTML forms directly from `DataItem` field definitions with 46+ field renderers in `framework/field_renderers/`.

**`FilterForm`** (`framework/filter_form.inc`) — search/filter forms that build SQL constraints from multi-field input.

**`DataView`** (`framework/data_view.inc`) — list views with pagination, sorting, and Excel export.

**Query classes** (`framework/query.inc`, `indexed_query.inc`, `grouped_query.inc`, `json_query.inc`) — typed wrappers around SQL queries returning `DataItem` instances.

### Component Module Structure

Each component lives in `cms/components/<name>/` and typically contains:
- `manifest.inc` — component metadata and registration
- `<name>_manager.inc` — core component logic
- `datamodel/` — `DataItem` subclasses
- `admin/` — admin views
- `handlers/` — event handler registrations
- `js/` — component JavaScript, served via `RewriteComponentResources`
- `css/` — component stylesheets, served via `RewriteComponentResources`
- `<name>_schema.sql` — database schema
- `<name>_upgrade_manager.inc` — schema migration logic

### URL Rewriting

IIS rules are in `web.config`; Apache rules live in `.htaccess`. Both servers support the same routing:

| URL pattern | Destination |
|---|---|
| `/<page_id>` (numeric) | `page.php?page_id=$1` |
| `/admin` | `page.php?identifier=index` (QoL redirect for missing trailing slash) |
| `/admin/<identifier>` | `admin/page.php?identifier=$1` |
| `/<identifier>` | `page.php?identifier=$1` |
| `/<section>/<identifier>` | `page.php?section=$1&identifier=$2` |
| `/<section>/<identifier>/<id>` | `page.php?section=$1&identifier=$2&id=$3` (`id` may be `*`) — RESTful API query |
| `/<section>/<identifier>/<id>/<relations>` | `page.php?…&relations=$4` — RESTful API query with relation traversal |
| `/action/<component>/<action>` | `action.php?component=$1&action=$2` |
| `/fakoli/…` | `resource.php?path=$0` (framework assets) |
| `/components/<name>/<path>` | `resource.php?component=$1&path=$2` (component assets) |

`.svn/` and `.git/` paths are blocked (403) by Apache rules.

## Line Endings

All files should use LF (`\n`) line endings. CRLF files are encountered occasionally (a legacy of Windows editing) and should be converted to LF when touched. When editing a CRLF file, convert it in the same commit using:

```bash
perl -i -pe 's/\r\n/\n/g' <file>
```

Note: CRLF endings will cause string-based search/replace tools (including the Edit tool) to fail to match multi-line patterns, so conversion is also a practical necessity when editing.

## PHP 8 Compatibility Notes

The current branch is fixing PHP 8 issues. Common patterns to watch for:
- Implicit conversions from `null` to string/int that are now deprecated/errors
- Array/string function behavior changes
- `match` is now a reserved keyword (avoid as variable/function name)
- Named arguments and constructor promotion are available but not yet used
