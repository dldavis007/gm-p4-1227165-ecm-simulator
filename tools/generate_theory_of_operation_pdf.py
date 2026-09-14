#!/usr/bin/env python3
"""Generate the stable publication PDF for THEORY_OF_OPERATION.md."""

from __future__ import annotations

import html
import re
from pathlib import Path

from reportlab import rl_config
from reportlab.lib import colors
from reportlab.lib.enums import TA_CENTER, TA_LEFT
from reportlab.lib.pagesizes import letter
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import inch
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.pdfbase import pdfmetrics
from reportlab.platypus import (
    BaseDocTemplate,
    Frame,
    KeepTogether,
    ListFlowable,
    ListItem,
    NextPageTemplate,
    PageBreak,
    PageTemplate,
    Paragraph,
    Spacer,
    Table,
    TableStyle,
)
from reportlab.platypus.tableofcontents import TableOfContents


ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "THEORY_OF_OPERATION.md"
OUTPUT = ROOT / "THEORY_OF_OPERATION.pdf"
GITHUB_BASE = (
    "https://github.com/dldavis007/gm-p4-1227165-ecm-simulator/blob/main/"
)

rl_config.invariant = 1


def register_fonts() -> tuple[str, str, str, str]:
    candidates = [
        Path("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"),
        Path("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"),
        Path("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"),
        Path("/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf"),
    ]
    if all(path.exists() for path in candidates):
        names = ("DejaVu", "DejaVu-Bold", "DejaMono", "DejaMono-Bold")
        for name, path in zip(names, candidates):
            pdfmetrics.registerFont(TTFont(name, str(path)))
        return names
    return ("Helvetica", "Helvetica-Bold", "Courier", "Courier-Bold")


BODY_FONT, BOLD_FONT, MONO_FONT, MONO_BOLD_FONT = register_fonts()


def normalize(text: str) -> str:
    replacements = {
        "\u2013": "-",
        "\u2014": "-",
        "\u2018": "'",
        "\u2019": "'",
        "\u201c": '"',
        "\u201d": '"',
        "\u2192": "->",
        "\u00a0": " ",
    }
    for old, new in replacements.items():
        text = text.replace(old, new)
    return text


TOKEN_RE = re.compile(r"(\[[^\]]+\]\([^)]+\)|`[^`]+`|\*\*[^*]+\*\*)")


def inline_markup(text: str) -> str:
    text = normalize(text)
    pieces: list[str] = []
    position = 0
    for match in TOKEN_RE.finditer(text):
        pieces.append(html.escape(text[position : match.start()]))
        token = match.group(0)
        if token.startswith("["):
            link_match = re.match(r"\[([^\]]+)\]\(([^)]+)\)", token)
            if link_match:
                label, target = link_match.groups()
                if not re.match(r"^[a-z]+://", target):
                    target = GITHUB_BASE + target
                pieces.append(
                    '<link href="%s" color="#22577A">%s</link>'
                    % (html.escape(target, quote=True), html.escape(label))
                )
        elif token.startswith("`"):
            pieces.append(
                '<font name="%s" color="#183B56">%s</font>'
                % (MONO_FONT, html.escape(token[1:-1]))
            )
        else:
            pieces.append("<b>%s</b>" % html.escape(token[2:-2]))
        position = match.end()
    pieces.append(html.escape(text[position:]))
    return "".join(pieces)


class PublicationDocTemplate(BaseDocTemplate):
    def __init__(self, filename: str, **kwargs: object) -> None:
        super().__init__(filename, **kwargs)
        frame = Frame(
            self.leftMargin,
            self.bottomMargin,
            self.width,
            self.height,
            id="normal",
        )
        self.addPageTemplates(
            [
                PageTemplate(id="cover", frames=frame, onPage=draw_cover_page),
                PageTemplate(id="body", frames=frame, onPage=draw_body_page),
            ]
        )

    def afterFlowable(self, flowable: object) -> None:
        if isinstance(flowable, Paragraph):
            level = getattr(flowable, "toc_level", None)
            if level is not None:
                text = flowable.getPlainText()
                key = "heading-%s" % self.seq.nextf("heading")
                self.canv.bookmarkPage(key)
                self.canv.addOutlineEntry(text, key, level=level, closed=False)
                self.notify("TOCEntry", (level, text, self.page, key))


def draw_cover_page(canvas: object, doc: object) -> None:
    canvas.saveState()
    canvas.setTitle("GM P4 1227165 ECM Simulator - Theory of Operation")
    canvas.setAuthor("dldavis007/gm-p4-1227165-ecm-simulator")
    canvas.setSubject("Evidence-supported firmware and simulator operation")
    canvas.setCreator("Repository publication generator")
    canvas.restoreState()


def draw_body_page(canvas: object, doc: object) -> None:
    canvas.saveState()
    width, height = letter
    canvas.setStrokeColor(colors.HexColor("#A8B3BD"))
    canvas.setLineWidth(0.4)
    canvas.line(doc.leftMargin, height - 0.48 * inch, width - doc.rightMargin, height - 0.48 * inch)
    canvas.setFont(BODY_FONT, 7.5)
    canvas.setFillColor(colors.HexColor("#52616B"))
    canvas.drawString(doc.leftMargin, height - 0.38 * inch, "GM P4 1227165 ECM Simulator - Theory of Operation")
    canvas.drawRightString(width - doc.rightMargin, 0.36 * inch, "Revision 1.0  |  Page %d" % doc.page)
    canvas.restoreState()


def styles() -> dict[str, ParagraphStyle]:
    sample = getSampleStyleSheet()
    return {
        "title": ParagraphStyle(
            "PublicationTitle",
            parent=sample["Title"],
            fontName=BOLD_FONT,
            fontSize=25,
            leading=30,
            textColor=colors.HexColor("#12344D"),
            alignment=TA_LEFT,
            spaceAfter=18,
        ),
        "subtitle": ParagraphStyle(
            "PublicationSubtitle",
            parent=sample["Normal"],
            fontName=BODY_FONT,
            fontSize=12,
            leading=18,
            textColor=colors.HexColor("#52616B"),
        ),
        "h2": ParagraphStyle(
            "Section",
            parent=sample["Heading1"],
            fontName=BOLD_FONT,
            fontSize=15,
            leading=19,
            textColor=colors.HexColor("#12344D"),
            spaceBefore=12,
            spaceAfter=7,
            keepWithNext=True,
        ),
        "h3": ParagraphStyle(
            "Subsection",
            parent=sample["Heading2"],
            fontName=BOLD_FONT,
            fontSize=11.5,
            leading=15,
            textColor=colors.HexColor("#22577A"),
            spaceBefore=9,
            spaceAfter=5,
            keepWithNext=True,
        ),
        "body": ParagraphStyle(
            "Body",
            parent=sample["BodyText"],
            fontName=BODY_FONT,
            fontSize=8.7,
            leading=12.3,
            textColor=colors.HexColor("#202A31"),
            spaceAfter=6,
        ),
        "bullet": ParagraphStyle(
            "BulletText",
            parent=sample["BodyText"],
            fontName=BODY_FONT,
            fontSize=8.5,
            leading=11.8,
            leftIndent=3,
        ),
        "table": ParagraphStyle(
            "TableText",
            parent=sample["BodyText"],
            fontName=BODY_FONT,
            fontSize=7.1,
            leading=9.2,
            textColor=colors.HexColor("#202A31"),
        ),
        "table_header": ParagraphStyle(
            "TableHeader",
            parent=sample["BodyText"],
            fontName=BOLD_FONT,
            fontSize=7.2,
            leading=9.3,
            textColor=colors.white,
        ),
        "toc_title": ParagraphStyle(
            "TOCTitle",
            parent=sample["Heading1"],
            fontName=BOLD_FONT,
            fontSize=18,
            textColor=colors.HexColor("#12344D"),
            alignment=TA_CENTER,
            spaceAfter=16,
        ),
        "note": ParagraphStyle(
            "Note",
            parent=sample["BodyText"],
            fontName=BODY_FONT,
            fontSize=8.4,
            leading=12,
            borderColor=colors.HexColor("#8DB8CE"),
            borderWidth=0.7,
            borderPadding=8,
            backColor=colors.HexColor("#EEF6FA"),
            textColor=colors.HexColor("#183B56"),
            spaceBefore=4,
            spaceAfter=9,
        ),
    }


def heading(text: str, style: ParagraphStyle, level: int) -> Paragraph:
    paragraph = Paragraph(inline_markup(text), style)
    paragraph.toc_level = level
    return paragraph


def parse_table(lines: list[str], table_styles: dict[str, ParagraphStyle]) -> Table:
    rows: list[list[Paragraph]] = []
    for row_number, line in enumerate(lines):
        cells = [cell.strip() for cell in line.strip().strip("|").split("|")]
        if row_number == 1 and all(re.fullmatch(r":?-{3,}:?", cell) for cell in cells):
            continue
        cell_style = table_styles["table_header"] if not rows else table_styles["table"]
        rows.append([Paragraph(inline_markup(cell), cell_style) for cell in cells])
    columns = len(rows[0])
    available = 7.1 * inch
    if columns == 2:
        widths = [1.35 * inch, available - 1.35 * inch]
    elif columns == 3:
        widths = [1.25 * inch, 2.35 * inch, available - 3.60 * inch]
    else:
        widths = [available / columns] * columns
    table = Table(rows, colWidths=widths, repeatRows=1, hAlign="LEFT")
    table.setStyle(
        TableStyle(
            [
                ("BACKGROUND", (0, 0), (-1, 0), colors.HexColor("#22577A")),
                ("VALIGN", (0, 0), (-1, -1), "TOP"),
                ("GRID", (0, 0), (-1, -1), 0.35, colors.HexColor("#B7C4CC")),
                ("ROWBACKGROUNDS", (0, 1), (-1, -1), [colors.white, colors.HexColor("#F4F7F9")]),
                ("LEFTPADDING", (0, 0), (-1, -1), 4),
                ("RIGHTPADDING", (0, 0), (-1, -1), 4),
                ("TOPPADDING", (0, 0), (-1, -1), 4),
                ("BOTTOMPADDING", (0, 0), (-1, -1), 4),
            ]
        )
    )
    return table


def markdown_story(source: str, style_map: dict[str, ParagraphStyle]) -> list[object]:
    lines = source.splitlines()
    story: list[object] = []
    index = 1  # publication title is represented by the cover
    paragraph_lines: list[str] = []

    def flush_paragraph() -> None:
        if paragraph_lines:
            story.append(Paragraph(inline_markup(" ".join(paragraph_lines)), style_map["body"]))
            paragraph_lines.clear()

    while index < len(lines):
        line = lines[index]
        stripped = line.strip()
        if not stripped:
            flush_paragraph()
            index += 1
            continue
        if stripped.startswith("## "):
            flush_paragraph()
            heading_text = stripped[3:]
            if heading_text.startswith("Appendix "):
                story.append(PageBreak())
            story.append(heading(heading_text, style_map["h2"], 0))
            index += 1
            continue
        if stripped.startswith("### "):
            flush_paragraph()
            story.append(heading(stripped[4:], style_map["h3"], 1))
            index += 1
            continue
        if stripped.startswith("|"):
            flush_paragraph()
            table_lines: list[str] = []
            while index < len(lines) and lines[index].strip().startswith("|"):
                table_lines.append(lines[index])
                index += 1
            story.append(parse_table(table_lines, style_map))
            story.append(Spacer(1, 7))
            continue
        if stripped.startswith("- "):
            flush_paragraph()
            items: list[ListItem] = []
            while index < len(lines) and lines[index].strip().startswith("- "):
                value = lines[index].strip()[2:]
                index += 1
                while index < len(lines) and lines[index].startswith("  "):
                    value += " " + lines[index].strip()
                    index += 1
                items.append(ListItem(Paragraph(inline_markup(value), style_map["bullet"])))
            story.append(ListFlowable(items, bulletType="bullet", leftIndent=16, bulletFontName=BODY_FONT, bulletFontSize=6))
            story.append(Spacer(1, 5))
            continue
        if re.match(r"^\d+\. ", stripped):
            flush_paragraph()
            items = []
            while index < len(lines) and re.match(r"^\d+\. ", lines[index].strip()):
                value = re.sub(r"^\d+\. ", "", lines[index].strip())
                index += 1
                while index < len(lines) and lines[index].startswith("  "):
                    value += " " + lines[index].strip()
                    index += 1
                items.append(ListItem(Paragraph(inline_markup(value), style_map["bullet"])))
            story.append(ListFlowable(items, bulletType="1", leftIndent=18, bulletFontName=BODY_FONT, bulletFontSize=7))
            story.append(Spacer(1, 5))
            continue
        if stripped.startswith("```"):
            flush_paragraph()
            language = stripped[3:].strip()
            index += 1
            code_lines: list[str] = []
            while index < len(lines) and not lines[index].strip().startswith("```"):
                code_lines.append(lines[index])
                index += 1
            index += 1
            if language == "mermaid":
                note = "Architecture boundary: Physical system / plant (F3-F4) -> raw HAL boundary (F2) -> firmware behavior (F0) -> raw HAL outputs, with schematic-proven connections retained as F1 evidence."
            else:
                note = "<br/>".join(html.escape(normalize(value)) for value in code_lines)
            story.append(Paragraph(note, style_map["note"]))
            continue
        paragraph_lines.append(stripped)
        index += 1
    flush_paragraph()
    return story


def build() -> None:
    style_map = styles()
    source = SOURCE.read_text(encoding="utf-8")
    doc = PublicationDocTemplate(
        str(OUTPUT),
        pagesize=letter,
        rightMargin=0.70 * inch,
        leftMargin=0.70 * inch,
        topMargin=0.68 * inch,
        bottomMargin=0.58 * inch,
        title="GM P4 1227165 ECM Simulator - Theory of Operation",
        author="dldavis007/gm-p4-1227165-ecm-simulator",
        subject="Evidence-supported firmware and simulator operation",
    )

    cover = [
        Spacer(1, 1.45 * inch),
        Paragraph("GM P4 1227165 ECM Simulator", style_map["title"]),
        Paragraph("Theory of Operation", ParagraphStyle("CoverSecond", parent=style_map["title"], fontSize=29, leading=34, textColor=colors.HexColor("#22577A"))),
        Spacer(1, 0.28 * inch),
        Table([["CONTROLLED ENGINEERING PUBLICATION"]], colWidths=[4.1 * inch], style=TableStyle([
            ("BACKGROUND", (0, 0), (-1, -1), colors.HexColor("#12344D")),
            ("TEXTCOLOR", (0, 0), (-1, -1), colors.white),
            ("FONTNAME", (0, 0), (-1, -1), BOLD_FONT),
            ("FONTSIZE", (0, 0), (-1, -1), 9),
            ("ALIGN", (0, 0), (-1, -1), "CENTER"),
            ("TOPPADDING", (0, 0), (-1, -1), 7),
            ("BOTTOMPADDING", (0, 0), (-1, -1), 7),
        ])),
        Spacer(1, 0.40 * inch),
        Paragraph("ECM service number 1227165  |  Supplied 9340 image", style_map["subtitle"]),
        Paragraph("Revision 1.0  |  14 September 2026", style_map["subtitle"]),
        Spacer(1, 1.15 * inch),
        Paragraph("Firmware authority: evidence/firmware/bua-hac.lst", style_map["subtitle"]),
        Spacer(1, 0.22 * inch),
        Paragraph("This publication describes evidence-supported processor-visible behavior and explicitly preserves unresolved custom-device, electrical, mechanical, power, and plant boundaries.", style_map["note"]),
        NextPageTemplate("body"),
        PageBreak(),
    ]

    toc = TableOfContents()
    toc.levelStyles = [
        ParagraphStyle("TOC1", fontName=BODY_FONT, fontSize=9, leading=13, leftIndent=8, firstLineIndent=-8, textColor=colors.HexColor("#12344D")),
        ParagraphStyle("TOC2", fontName=BODY_FONT, fontSize=8, leading=11, leftIndent=22, firstLineIndent=-8, textColor=colors.HexColor("#52616B")),
    ]
    story: list[object] = cover + [Paragraph("Contents", style_map["toc_title"]), toc, PageBreak()]
    story.extend(markdown_story(source, style_map))
    doc.multiBuild(story)
    print(OUTPUT)


if __name__ == "__main__":
    build()
