function Div(el)
  if el.classes[1] == "warning" then
    -- insert element in front
    table.insert(
      el.content, 1,
      pandoc.RawBlock("latex", "\\noindent\\fcolorbox{red}{yellow}{\\minipage[t]{\\linewidth}"))
    -- insert element at the back
    table.insert(
      el.content,
      pandoc.RawBlock("latex", "\\endminipage}"))
  end
  return el
end