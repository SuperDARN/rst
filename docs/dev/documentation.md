# Documenting in RST 

Currently RST is in a transition of documentation between the old system using XML to Markdown using read the docs as serving platform for the documentation. 

Currently the read the docs documents:
- Installation
  - Linux
  - Mac
  - Windows (coming soon)
- RST Tutorials 
- DMAP format (coming soon)

The XML documentation currently hosts:
- RST binary information 
- Historical notes on DMAP and RST 

## Documentation in Markdown

### Need to know 
[Read the docs](https://readthedocs.org/) is a webpage that serves your documentation onto a pre-formatted webpage with some additive features.  
Read the docs loads in formatted HTML files build with [MkDocs](https://www.mkdocs.org/) or [SPHINX](http://www.sphinx-doc.org/en/master/) using the `readthedocs` theme. 
MkDocs is what RST readthedocs builds off of, that uses [MarkDown](https://www.markdownguide.org/) (.md) files found in `./docs` folder in RST. 

Markdown is a markup HTML format for quicker documentation, it is supported by GitHub and widely used for writing README.md. 

### How to Add Documentation
To add documentation in the read the docs:
1. Go to `./docs`
2. Go to or create the appropriate folder where you documentation will be stored in
3. Create or add to the markdown file you want to add your documentation to
  - Markdown files end in .md 
4. Write your documentation using [Markdown](https://www.markdownguide.org/cheat-sheet) syntax 
5. If you added a new file to the folder system add it to the `./mkdocs.yml` configure file and `./index.md` file 

Viewing the Documentation:
1. install `mkdocs` via `pip install mkdocs` 
2. view the documentation via `mkdocs serve` 
  - click on the local host address shown in the terminal

Other methods of viewing the documentation is pushing the documentation to GitHub and building it on readthedocs. 
By pushing it to GitHub it may automatically build the documentation if webhooks are correctly setup.  
 
## Documentation in RST source code 

### Need to know 
RST documentation was first written in Requests for Comments (rfc) that is no longer being updated and later moved to XML to document the binaries in RST. 
The XML code also allows for command line help formatting using the documentation build scripts. 

### How to Add Documentation
Adding to pre-existing documentation:
1. Find the XML file associated to the binary you changed/updated. This will be where you find  
2. 

Adding new documentation:
1. 
