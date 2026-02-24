" Silence `:make`, only capture errors in the quickfix window.
:set makeprg=make\ -s
:setlocal makeprg=make\ -s

map <Leader>t :wall!\|:make clean\|:make\|:!build/snaker<cr>
