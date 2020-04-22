function Div(el)
  if el.classes[1] == "proposition" then
    -- insert element in front
    table.insert(
      el.content, 1,
      pandoc.RawBlock("latex", "\\fbox{\\begin{minipage}{\\textwidth}\\ttfamily"))
    -- insert element at the back
    table.insert(
      el.content,
      pandoc.RawBlock("latex", "\\end{minipage}}"))
  end
  return el
end