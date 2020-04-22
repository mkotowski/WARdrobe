return {
  {
    Str = function (elem)
      if elem.text == "<mark>" then
        return pandoc.Emph {pandoc.Str "<span style='background-color:yellow'>"}
      else
        return elem
      end

      if elem.text == "</mark>" then
        return pandoc.Emph {pandoc.Str "</span>"}
      else
        return elem
      end
    end,
  }
}