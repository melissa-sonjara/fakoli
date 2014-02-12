<h2>About Taxonomies &amp; Bindings</h2>
<p>Taxonomies and bindings are two different but related methods for classifying the content on your site.</p>
<h3>Taxonomies</h3>
<p>Taxonomies provide a mechanism for adding classification metadata to site content items that have registered
themselves as "taxonomy-enabled". This includes all the standard Fakoli CMS items, including pages, articles,
blogs, images, documents, etc.</p>
<p>A taxonomy consists of a set of <strong>terms</strong>. Each term can optionally have a definition (providing
you with a glossary). The taxonomy can then be associated with any number of content classes. Once a taxonomy 
is associated in this fashion, a field will be added to the content entry forms for the associated content types,
allowing content editors to classify the content according to the list of taxonomy terms.</p>
<p>Taxonomies can be used programmatically via the TaxonomyManager API, for instance to provide links to related
data or content. Additionally taxonomies can interact automatically with the site search feature to provide 
facet filtering of search results.</p>
<h3>Bindings</h3>
<p>Bindings operate in a similar manner to taxonomies, but instead of a list of user-defined terms, they provide
a mapping to items defined in your applications data model. When developing your application's custom components,
your component can register classes in their data model as bindable targets. On the binding page you can then
select which content classes can be 'bound' to those data items. Any content items that are taxonomy-enabled can
also be bound in this fashion.</p>
