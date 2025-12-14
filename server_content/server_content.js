// Typing animation for each .cmd element
(function(){
  const lines = Array.from(document.querySelectorAll('.line'));
  const typeSpeed = 18; // ms per char
  const gapBetweenLines = 220; // ms
  // helper: type text into element char by char
  function typeInto(el, text, speed){
    return new Promise(resolve => {
      el.textContent = '';
      let i = 0;
      function step(){
        if(i <= text.length-1){
          el.textContent += text[i++];
          setTimeout(step, speed);
        } else {
          resolve();
        }
      }
      step();
    });
  }

  // Sequentially type each visible link command and then show result below
  (async function play(){
    for(let i=0;i<lines.length;i++){
      const line = lines[i];
      const cmdEl = line.querySelector('.cmd');
      const txt = cmdEl.dataset.text || line.dataset.cmd || '';
      await typeInto(cmdEl, txt, typeSpeed);
      // show simulated command output (result) if provided
      const anchor = line.closest('.line-link');
      const resultHtml = line.dataset.result;
      if(resultHtml){
        // avoid inserting multiple times
        if(!anchor.nextElementSibling || !anchor.nextElementSibling.classList.contains('result')){
          const out = document.createElement('div');
          out.className = 'result';
          out.innerHTML = resultHtml;
          anchor.parentNode.insertBefore(out, anchor.nextSibling);
        }
      }
      // leave caret blinking a bit, then continue
      await new Promise(r => setTimeout(r, gapBetweenLines));
    }
  })();

  // On hover, quickly reveal full command if not yet typed
  lines.forEach(line => {
    const cmdEl = line.querySelector('.cmd');
    const full = cmdEl.dataset.text || '';
    let revealTimer = null;
    line.addEventListener('mouseenter', () => {
      if(cmdEl.textContent === full) return;
      // reveal immediately
      cmdEl.textContent = full;
      if(revealTimer) { clearTimeout(revealTimer); revealTimer = null; }
    });
    line.addEventListener('mouseleave', () => {
      // optionally, do nothing to keep full text visible
    });
  });

  // Make the entire .line clickable (already anchor wraps it)
  // Add keyboard activation for accessibility
  document.querySelectorAll('.line-link').forEach(a => {
    a.addEventListener('keydown', (e) => {
      if(e.key === 'Enter' || e.key === ' ') {
        e.preventDefault();
        a.click();
      }
    });
    // Make them focusable for keyboard 0xPr0F3ss0rs
    a.setAttribute('tabindex', '0');
  });

})();